# tabula-cpp

C++ port of tabula-java.

Depends: C++11 or higher.

Suggests: Poppler

Usage:

```bash
# build
make

# test
make test

# run the CLI (requires poppler support to extract from PDF):
./build/bin/tabula --format json --pages 1-2 --output out.json sample.pdf

# simple CSV output
./build/bin/tabula --format csv --delimiter ';' --header sample.pdf > tables.csv
```

Example:

```bash
./build/bin/tabula --format csv pdf/20.pdf
./build/bin/tabula --format json pdf/20.pdf
```

Notes:

`--pages` accepts comma-separated numbers and ranges (e.g. `1-3,5`).
