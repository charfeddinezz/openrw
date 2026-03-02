#!/usr/bin/env python3
"""Utility script to launch the OpenRW game binary."""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path


DEFAULT_CANDIDATES = (
    Path("build/rwgame/rwgame"),
    Path("build/rwgame/Debug/rwgame"),
    Path("build/rwgame/Release/rwgame"),
    Path("build/rwgame/RelWithDebInfo/rwgame"),
    Path("rwgame/rwgame"),
)


def find_binary(explicit_path: str | None) -> Path:
    if explicit_path:
        candidate = Path(explicit_path).expanduser()
        if candidate.exists() and os.access(candidate, os.X_OK):
            return candidate
        raise FileNotFoundError(f"Binary not found or not executable: {candidate}")

    for candidate in DEFAULT_CANDIDATES:
        if candidate.exists() and os.access(candidate, os.X_OK):
            return candidate

    tried = "\n".join(f"  - {path}" for path in DEFAULT_CANDIDATES)
    raise FileNotFoundError(
        "Could not locate the rwgame executable.\n"
        "Build the project first and/or pass --binary.\n"
        f"Checked:\n{tried}"
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run the OpenRW game executable from common build directories."
    )
    parser.add_argument(
        "--binary",
        help="Path to rwgame executable (overrides auto-detection).",
    )
    parser.add_argument(
        "--cwd",
        default=".",
        help="Working directory used when launching the game (default: current dir).",
    )
    parser.add_argument(
        "game_args",
        nargs=argparse.REMAINDER,
        help="Arguments to pass to rwgame. Use '--' before game args.",
    )

    args = parser.parse_args()

    try:
        binary = find_binary(args.binary)
    except FileNotFoundError as error:
        print(error, file=sys.stderr)
        return 1

    game_args = list(args.game_args)
    if game_args and game_args[0] == "--":
        game_args = game_args[1:]

    command = [str(binary), *game_args]
    print(f"Launching: {' '.join(command)}")

    try:
        completed = subprocess.run(command, cwd=args.cwd)
    except OSError as error:
        print(f"Failed to launch {binary}: {error}", file=sys.stderr)
        return 1

    return completed.returncode


if __name__ == "__main__":
    raise SystemExit(main())
