#!/usr/bin/env bash
set -euo pipefail

VENV_DIR=${1:-.venv}
PYTHON_BIN=${PYTHON_BIN:-python3}

if ! command -v "$PYTHON_BIN" >/dev/null 2>&1; then
    echo "Error: python interpreter '$PYTHON_BIN' not found" >&2
    exit 1
fi

if [ ! -d "$VENV_DIR" ]; then
    echo "[+] Creating virtual environment in $VENV_DIR"
    "$PYTHON_BIN" -m venv "$VENV_DIR"
else
    echo "[+] Virtual environment $VENV_DIR already exists"
fi

VENV_PY="$VENV_DIR/bin/python"

if [ ! -x "$VENV_PY" ]; then
    echo "Error: expected python executable at $VENV_PY" >&2
    exit 1
fi

echo "[+] Upgrading pip in the virtual environment"
"$VENV_PY" -m pip install --upgrade pip

echo "[+] Installing Python dependencies from requirements.txt"
"$VENV_PY" -m pip install -r requirements.txt

echo
cat <<EOM
Virtual environment ready.
Activate it in your shell:
  source $VENV_DIR/bin/activate
Then build the project:
  cmake -S . -B build
  cmake --build build
To run the example without installing the module:
  PYTHONPATH=build python examples/simple_usage.py
EOM
