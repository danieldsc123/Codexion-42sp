#!/usr/bin/env python3
"""Black-box subject checks; run from the repository root after make."""
import os
import re
import subprocess
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BINARY = os.environ.get('CODEXION_BIN', str(ROOT / 'codexion'))
LINE = re.compile(r'^(\d+) (\d+) (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$')


class SimulationTests(unittest.TestCase):
    def run_sim(self, args, timeout=15):
        before = time.monotonic()
        result = subprocess.run([BINARY, *map(str, args)], capture_output=True,
                                text=True, timeout=timeout)
        elapsed = (time.monotonic() - before) * 1000
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stderr, '')
        logs = []
        for line in result.stdout.splitlines():
            match = LINE.fullmatch(line)
            self.assertIsNotNone(match, line)
            timestamp, coder, event = match.groups()
            logs.append((int(timestamp), int(coder), event))
        self.assertEqual([row[0] for row in logs], sorted(row[0] for row in logs))
        return logs, elapsed

    def check_success(self, args):
        n, burnout, compile_ms, debug, refactor, target, cooldown, _ = args
        logs, elapsed = self.run_sim(args)
        starts = {i: [] for i in range(1, n + 1)}
        takes = {i: 0 for i in starts}
        phases = {i: None for i in starts}
        phase_at = {i: 0 for i in starts}
        available = [0] * n
        for timestamp, coder, event in logs:
            self.assertIn(coder, starts)
            self.assertNotEqual(event, 'burned out', (args, logs[-8:]))
            if event == 'has taken a dongle':
                self.assertIn(phases[coder], (None, 'is refactoring'))
                takes[coder] += 1
                self.assertLessEqual(takes[coder], 2)
            elif event == 'is compiling':
                self.assertEqual(takes[coder], 2)
                takes[coder] = 0
                if starts[coder]:
                    self.assertLess(timestamp - starts[coder][-1], burnout)
                    self.assertEqual(phases[coder], 'is refactoring')
                    self.assertGreaterEqual(timestamp - phase_at[coder], refactor)
                else:
                    self.assertLess(timestamp, burnout)
                for resource in (coder - 1, coder % n):
                    self.assertGreaterEqual(timestamp, available[resource], args)
                    available[resource] = timestamp + compile_ms + cooldown
                starts[coder].append(timestamp)
                phases[coder], phase_at[coder] = event, timestamp
            elif event == 'is debugging':
                self.assertEqual(phases[coder], 'is compiling')
                self.assertGreaterEqual(timestamp - phase_at[coder], compile_ms)
                phases[coder], phase_at[coder] = event, timestamp
            elif event == 'is refactoring':
                self.assertEqual(phases[coder], 'is debugging')
                self.assertGreaterEqual(timestamp - phase_at[coder], debug)
                phases[coder], phase_at[coder] = event, timestamp
        for coder in starts:
            self.assertGreaterEqual(len(starts[coder]), target, args)
            self.assertGreaterEqual(elapsed + 2, starts[coder][target - 1] + compile_ms)

    def test_valid_schedules(self):
        for policy in ('fifo', 'edf'):
            for n in (2, 3, 4, 5, 10, 31):
                with self.subTest(n=n, policy=policy):
                    self.check_success([n, 2000, 12, 4, 3, 5, 6, policy])

    def test_subject_example(self):
        for policy in ('fifo', 'edf'):
            self.check_success([5, 800, 200, 100, 50, 3, 20, policy])

    def test_zero_durations(self):
        for policy in ('fifo', 'edf'):
            self.check_success([5, 2000, 0, 0, 0, 30, 0, policy])

    def test_cooldown(self):
        self.check_success([5, 2000, 5, 0, 0, 8, 20, 'edf'])

    def test_burnout(self):
        for args in ([1, 60, 10, 5, 5, 1, 0, 'fifo'],
                     [2, 40, 100, 0, 0, 2, 0, 'edf'],
                     [3, 0, 10, 0, 0, 1, 0, 'edf']):
            logs, _ = self.run_sim(args)
            deaths = [row for row in logs if row[2] == 'burned out']
            self.assertEqual(len(deaths), 1)
            self.assertEqual(logs[-1], deaths[0])
            timestamp, coder, _ = deaths[0]
            last_start = max([t for t, c, e in logs if c == coder and e == 'is compiling'] or [0])
            self.assertGreaterEqual(timestamp - last_start, args[1])
            self.assertLessEqual(timestamp - last_start - args[1], 10)
            if args[0] == 1:
                self.assertEqual(sum(e == 'has taken a dongle' for _, _, e in logs), 1)
                self.assertFalse(any(e == 'is compiling' for _, _, e in logs))

    def test_zero_goal(self):
        logs, _ = self.run_sim([1, 0, 0, 0, 0, 0, 0, 'edf'])
        self.assertEqual(logs, [])

    def test_rejections(self):
        base = ['5', '800', '200', '100', '50', '3', '20', 'fifo']
        bad = [[], base[:-1], base + ['extra']]
        for i in range(7):
            for value in ('-1', '+1', '', ' 2', '2 ', '1.5', '12x', '2147483648'):
                args = base.copy()
                args[i] = value
                bad.append(args)
        for value in ('FIFO', 'EDF', '0', 'edf ', ''):
            bad.append(base[:-1] + [value])
        bad.append(['0'] + base[1:])
        for args in bad:
            result = subprocess.run([BINARY, *args], capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 1, args)
            self.assertEqual(result.stdout, '')
            self.assertEqual(result.stderr, 'Error: invalid arguments\n')


if __name__ == '__main__':
    unittest.main(verbosity=2)
