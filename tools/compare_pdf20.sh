#!/usr/bin/env bash
set -eu

# stop if mvn is not found
if ! command -v mvn &> /dev/null
then
    echo "mvn could not be found, please install Maven to proceed."
    exit 1
fi

# Compare Java tabula and C++ tabula output on resources/technology/tabula/20.pdf
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PDF_DIR="$ROOT/pdf"
JAR_DIR="$ROOT/java/target"

# Ensure tools output dir exists
mkdir -p "$ROOT/tools"

JAVASCRIPT_BASE="$ROOT/tools/java_out"
CPPBASE="$ROOT/tools/cpp_out"

# Build Java (maven) - build every run so we exercise the java build here too
echo "Building Java jar..."
(cd "$ROOT/java" && mvn -q -DskipTests package)

# Build C++ CLI (make) once
echo "Building C++ tabula..."
(cd "$ROOT" && ./configure >/dev/null 2>&1 || true)
# Build default targets from the repository root (no short 'tabula' target exists)
(cd "$ROOT" && make -j2 >/dev/null)

if [ ! -x "$ROOT/build/bin/tabula" ]; then
  echo "C++ tabula binary not found at build/bin/tabula";
  exit 3;
fi

# Iterate over all PDFs in pdf/ and compare outputs
failures=()
shopt -s nullglob
PDF_FILES=("$PDF_DIR"/*.pdf)
if [ ${#PDF_FILES[@]} -eq 0 ]; then
  echo "No PDF files found in $PDF_DIR";
  exit 0;
fi

for PDF in "${PDF_FILES[@]}"; do
  name=$(basename "$PDF")
  stem="${name%.*}"
  JAVA_OUT="$JAVASCRIPT_BASE"_${stem}.json
  CPP_OUT="$CPPBASE"_${stem}.json

  echo "\nProcessing: $name"

  # Run Java CLI for this PDF (don't let a single failure stop the loop)
  echo "  Running Java tabula..."
  if ! java -jar "$JAR_DIR/tabula-1.0.6-SNAPSHOT-jar-with-dependencies.jar" -f JSON "$PDF" > "$JAVA_OUT" 2> >(sed 's/^/    [java] /' >&2); then
    echo "    Java tabula failed for $name -- skipping comparison"
    failures+=("$name (java-failed)")
    continue
  fi

  # Run C++ CLI for this PDF
  echo "  Running C++ tabula..."
  if ! ./build/bin/tabula --format json "$PDF" > "$CPP_OUT" 2> >(sed 's/^/    [cpp] /' >&2); then
    echo "    C++ tabula failed for $name -- skipping comparison"
    failures+=("$name (cpp-failed)")
    continue
  fi

  # Compare using comparator
  echo "  Comparing JSON outputs..."
  if python3 "$ROOT/tools/compare_jsons.py" "$JAVA_OUT" "$CPP_OUT"; then
    echo "  OK: match (normalized)"
  else
    echo "  Differences found — showing pretty diff for $name"
    JAVA_PRETTY=$(mktemp)
    CPP_PRETTY=$(mktemp)

  python3 - <<PY > "$JAVA_PRETTY"
import json,sys
try:
  obj=json.load(open('$JAVA_OUT'))
  print(json.dumps(obj,sort_keys=True,indent=2,ensure_ascii=False))
except Exception as e:
  raw=open('$JAVA_OUT','rb').read().decode('utf-8',errors='backslashreplace')
  sys.stderr.write('  [warn] failed to parse JSON: %s\n' % e)
  print(json.dumps({'_raw': raw}, ensure_ascii=False, indent=2))
PY

  python3 - <<PY > "$CPP_PRETTY"
import json,sys
try:
  obj=json.load(open('$CPP_OUT'))
  print(json.dumps(obj,sort_keys=True,indent=2,ensure_ascii=False))
except Exception as e:
  raw=open('$CPP_OUT','rb').read().decode('utf-8',errors='backslashreplace')
  sys.stderr.write('  [warn] failed to parse JSON: %s\n' % e)
  print(json.dumps({'_raw': raw}, ensure_ascii=False, indent=2))
PY

    echo "--- Java output (normalized)"
    echo "+++ C++ output (normalized)"
    diff -u --label "java/$name" "$JAVA_PRETTY" --label "cpp/$name" "$CPP_PRETTY" || true

    rm -f "$JAVA_PRETTY" "$CPP_PRETTY"
    failures+=("$name (diff)")
  fi
done

if [ ${#failures[@]} -ne 0 ]; then
  echo "\nComparison finished: differences found in ${#failures[@]} file(s):"
  for f in "${failures[@]}"; do echo " - $f"; done
  exit 2
else
  echo "\nComparison finished: all files matched (normalized)"
fi
