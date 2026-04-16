# Pseudo Code → C Compiler

A full-stack web application that translates a custom English-like **pseudocode** into valid **C source code** using a real compiler pipeline (Flex + Bison + C), served through a Node.js/Express backend and a clean browser-based UI.

---

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Architecture](#architecture)
- [Compiler Pipeline (Deep Dive)](#compiler-pipeline-deep-dive)
- [Pseudocode Grammar Reference](#pseudocode-grammar-reference)
- [Web Stack](#web-stack)
- [Data Flow](#data-flow)
- [Setup & Running](#setup--running)
- [Known Vulnerabilities & Issues](#known-vulnerabilities--issues)
- [Limitations](#limitations)

---

## Overview

The user types pseudocode into a browser textarea. When **RUN** is clicked, the pseudocode is sent to a local Express server which writes it to disk, invokes the native compiler binary (via WSL on Windows), and returns the generated C code back to the browser — all in real time.

**Example:**

```
START
  VAR x AS INT
  SET x TO 1
  WHILE x <= 3
    IF x == 2
      SHOW "Target Reached"
    ENDIF
    SET x TO x + 1
  DONE
STOP
```

**Outputs:**

```c
#include <stdio.h>

int main() {
    int x;
    x = 1;
    while ((x <= 3)) {
        if ((x == 2)) {
            printf("%s\n", "Target Reached");
        }
        x = (x + 1);
    }
    return 0;
}
```

---

## Project Structure

```
Psudo_code_to_c/
│
├── index.html              # Single-page frontend UI
├── style.css               # Frontend styles
├── script.js               # Frontend JavaScript (fetch, UI state)
├── image.png               # Favicon / branding asset
├── package.json            # Root-level Node deps (ejs)
│
├── server/                 # Express backend
│   ├── index.js            # Server entry point (port 8000)
│   ├── package.json        # Server deps (express, cors, nodemon)
│   └── api/
│       ├── api.js          # POST /run — orchestrates compile+respond
│       └── log.js          # Request logging middleware
│
└── logic/                  # The C compiler (Flex + Bison)
    ├── lexer.l             # Flex lexer: tokenises pseudocode
    ├── parser.y            # Bison grammar: builds the AST
    ├── ast.h / ast.c       # AST node definitions & constructors
    ├── symtab.h / symtab.c # Symbol table (variable registry)
    ├── codegen.h / codegen.c # AST → C source code emitter
    ├── main.c              # Compiler entry point (calls yyparse)
    ├── Makefile            # Build system: flex → bison → gcc
    ├── Rules               # Human-readable keyword mapping table
    ├── .test.txt           # Ephemeral: pseudocode input written by server
    └── .output.txt         # Ephemeral: generated C code, read by server
```

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         BROWSER (Client)                        │
│  index.html + style.css + script.js                             │
│                                                                 │
│  [ Pseudocode Textarea ] ──RUN──► fetch POST /run              │
│                                        │                        │
│  [ C Output Textarea ]  ◄── res.json ──┘                        │
└─────────────────────────────────────────────────────────────────┘
                               │ HTTP (localhost:8000)
┌─────────────────────────────────────────────────────────────────┐
│                   EXPRESS SERVER  (server/)                     │
│                                                                 │
│  index.js → logging middleware → POST /run handler (api.js)    │
│                                                                 │
│  1. writeFile("logic/.test.txt", pseudocode)                   │
│  2. Build WSL path from Windows path                           │
│  3. exec(`wsl bash -c "./my_compiler < .test.txt > .output.txt"`)
│  4. readFileSync("logic/.output.txt")                          │
│  5. res.send({ output: <C code string> })                      │
└─────────────────────────────────────────────────────────────────┘
                               │ WSL subprocess
┌─────────────────────────────────────────────────────────────────┐
│               COMPILER BINARY  (logic/my_compiler)             │
│                                                                 │
│  stdin (pseudocode)                                             │
│      │                                                          │
│      ▼                                                          │
│  ┌─────────┐    tokens   ┌─────────┐    AST   ┌────────────┐  │
│  │  Flex   │ ──────────► │  Bison  │ ────────► │  codegen  │  │
│  │ lexer.l │             │parser.y │           │ codegen.c │  │
│  └─────────┘             └─────────┘           └────────────┘  │
│                               │                      │          │
│                          symtab.c               stdout (C code) │
└─────────────────────────────────────────────────────────────────┘
```

---

## Compiler Pipeline (Deep Dive)

### 1. Lexer — `lexer.l` (Flex)

Converts raw pseudocode text into a stream of tokens.

| Pattern | Token |
|---|---|
| `START`, `STOP` | `START`, `STOP` |
| `VAR`, `AS`, `INT` | `VAR`, `AS`, `INT` |
| `SET`, `TO` | `SET`, `TO` |
| `SHOW`, `GET` | `SHOW`, `GET` |
| `IF`, `ELSE`, `ENDIF` | `IF`, `ELSE`, `ENDIF` |
| `WHILE`, `DONE` | `WHILE`, `DONE` |
| `==`, `!=`, `<=`, `>=`, `<`, `>` | `EQ`, `NEQ`, `LE`, `GE`, `LT`, `GT` |
| `+`, `-`, `*`, `/` | `PLUS`, `MINUS`, `MULT`, `DIV` |
| `[0-9]+` | `NUMBER` (integer literal) |
| `[a-zA-Z]+` | `IDENTIFIER` (variable names) |
| `"..."` | `STRING` (quoted string literal) |
| Whitespace / newlines | Silently ignored |

### 2. Parser — `parser.y` (Bison)

Applies LALR(1) grammar rules to the token stream and constructs an **Abstract Syntax Tree (AST)**.

**Grammar summary:**
```
program     → START statements STOP
statements  → statements statement | ε
statement   → declaration | assignment | print_stmt |
              input_stmt | if_stmt | while_stmt
declaration → VAR IDENTIFIER AS INT
assignment  → SET IDENTIFIER TO expression
print_stmt  → SHOW STRING | SHOW expression
input_stmt  → GET IDENTIFIER
if_stmt     → IF condition statements [ELSE statements] ENDIF
while_stmt  → WHILE condition statements DONE
expression  → NUMBER | IDENTIFIER | expr OP expr | (expr)
condition   → expression RELOP expression
```

Operator precedence: `+`, `-` < `*`, `/` (left-associative).

### 3. AST — `ast.h` / `ast.c`

A single unified `ASTNode` struct covers all node types via tagged union-style fields:

| NodeType | Key Fields Used |
|---|---|
| `NODE_PROGRAM` | `left` → first statement |
| `NODE_BLOCK` | `left` → first statement in block |
| `NODE_VAR_DECL` | `str_val` → variable name |
| `NODE_ASSIGN` | `str_val` → var name, `left` → expr |
| `NODE_PRINT` | `left` → expression/string |
| `NODE_INPUT` | `str_val` → variable name |
| `NODE_IF` | `cond`, `then_branch`, `else_branch` |
| `NODE_WHILE` | `cond`, `then_branch` |
| `NODE_BINOP` | `str_val` → operator, `left`, `right` |
| `NODE_NUM` | `int_val` |
| `NODE_ID` | `str_val` → identifier name |
| `NODE_STR` | `str_val` → quoted string |

Statements at the same level are linked via the `next` pointer (intrusive linked list).

### 4. Symbol Table — `symtab.h` / `symtab.c`

A simple fixed-size flat array of `{ name, type }` pairs.  
Only `"int"` type is supported. Used during parsing (declaration phase) but **not validated during code generation** (undeclared variable usage is not caught).

### 5. Code Generator — `codegen.c`

Performs a recursive depth-first traversal of the AST and emits C source code to `stdout`. The output is a complete, standalone `.c` file with:
- `#include <stdio.h>`
- `int main() { ... return 0; }`
- Indentation managed via an `indent_level` integer

---

## Pseudocode Grammar Reference

| Pseudocode | Generated C | Purpose |
|---|---|---|
| `START` / `STOP` | `int main() { ... }` | Program delimiters |
| `VAR x AS INT` | `int x;` | Integer variable declaration |
| `SET x TO 5` | `x = 5;` | Assignment |
| `SET x TO x + 1` | `x = (x + 1);` | Arithmetic assignment |
| `SHOW "Hello"` | `printf("%s\n", "Hello");` | String print |
| `SHOW x` | `printf("%d\n", x);` | Integer print |
| `GET x` | `scanf("%d", &x);` | Integer input |
| `IF cond ... ENDIF` | `if (...) { ... }` | Conditional |
| `IF cond ... ELSE ... ENDIF` | `if (...) { } else { }` | If-else |
| `WHILE cond ... DONE` | `while (...) { ... }` | Loop |

---

## Web Stack

### Frontend (`index.html`, `style.css`, `script.js`)

- Pure HTML/CSS/JS — no framework.
- Font: **Monoton** (Google Fonts) for the header.
- **Material Symbols Outlined** for the close icon.
- Two `<textarea>` elements side-by-side: pseudocode input (left) and C output (right).
- A **mutex lock** (`let lock = false`) prevents double-submission while a request is in flight.
- The **RUN** button enters a "Compiling…" state (dimmed, pointer-events disabled) during the request.
- An alert overlay shows on invalid/undefined output.

### Backend (`server/`)

| File | Responsibility |
|---|---|
| `index.js` | Express app setup: CORS, JSON/text body parsers, EJS view engine, mounts router at `/run`, listens on port **8000** |
| `api/api.js` | `POST /run` handler: writes pseudocode to `.test.txt`, converts Windows path to WSL path, runs `my_compiler` binary in WSL subprocess, reads `.output.txt`, returns JSON |
| `api/log.js` | Middleware that appends `timestamp + URL` to `server/logs/.logs.txt` on every request |

**Dependencies:**

| Package | Version | Use |
|---|---|---|
| `express` | ^5.2.1 | HTTP framework |
| `cors` | ^2.8.6 | Enable cross-origin requests from the standalone HTML file |
| `nodemon` | ^3.1.14 | Dev: auto-restart on file change |

---

## Data Flow

```
1. User types pseudocode in browser textarea

2. Clicks RUN → script.js sends:
   POST http://localhost:8000/run
   Content-Type: text/plain
   Body: <pseudocode string>

3. Express receives body as plain text (express.text() middleware)

4. api.js writes body → logic/.test.txt  (disk)

5. api.js resolves Windows absolute path → WSL /mnt/... path

6. api.js executes:
   wsl bash -c "cd <dir> && ./my_compiler < .test.txt > .output.txt"

7. my_compiler (Flex+Bison):
   stdin → Lexer → Parser → AST → codegen → stdout

8. Compiler stdout is redirected to logic/.output.txt

9. api.js readFileSync(".output.txt") → string

10. res.send({ msg: "Executed Successful", output: "<C code>" })

11. script.js receives JSON, displays output field in browser
```

---

## Setup & Running

### Prerequisites

- **Windows with WSL** (Ubuntu recommended) — the compiler binary runs inside WSL
- **Node.js** (v18+) for the Express server
- **GCC, Flex, Bison** installed inside WSL

### Build the Compiler (inside WSL)

```bash
cd /mnt/d/Projects/Psudo_code_to_c/logic
make
# Produces: my_compiler binary
```

To rebuild from scratch:

```bash
make clean && make
```

### Start the Server (Windows PowerShell / CMD)

```powershell
cd d:\Projects\Psudo_code_to_c\server
npm install
npm start        # uses nodemon — auto-restarts on changes
```

Server listens at `http://localhost:8000`.

### Open the Frontend

Open `d:\Projects\Psudo_code_to_c\index.html` directly in your browser  
(or serve it statically — it fetches `http://localhost:8000/run` via CORS).

---

## Vulnerabilities & Fixes

> All items below have been patched except CORS (intentionally left open).

---

### ✅ Fixed — `exec()` replaced with `execFile()` · Race Condition · Temp File Cleanup

**File:** `server/api/api.js`

**Problems fixed:**
1. `exec()` spawned a shell on the Node side, making the command string an injection surface.
2. Both `.test.txt` and `.output.txt` were singleton files — concurrent requests trampled each other.
3. Output was read with blocking `readFileSync` and temp files were never deleted.

**Fix:** Switched to `execFile('wsl', [...args])` (no Node-side shell). Each request now generates a UUID and writes to `logic/.tmp_input_<uuid>.txt` and `logic/.tmp_output_<uuid>.txt`. Both files are deleted in a `finally` block after the response is sent regardless of success or failure. All file I/O is now async.

```js
// Before
exec(`wsl bash -c "cd ${compileDir} && ./my_compiler < ${wslPath} > ${outputPath}"`, ...);

// After
const id = randomUUID();
execFile('wsl', ['bash', '-c', `cd '${wslDir}' && ./my_compiler < '${wslInput}' > '${wslOutput}'`], async (...) => {
    // ... read, respond, then:
    await unlink(inputFilePath).catch(() => {});
    await unlink(outputFilePath).catch(() => {});
});
```

---

### ✅ Fixed — Input Size Limit

**File:** `server/index.js`

Added a `50kb` cap to `express.text()` to prevent DoS via oversized payloads:

```js
// Before
server.use(express.text())

// After
server.use(express.text({ limit: '50kb' }))
```

---

### ✅ Fixed — Broken JavaScript in `script.js`

**File:** `script.js`

Three bugs in `showalert()` / `closealert()`:

| Bug | Fix |
|---|---|
| `getElementsByTagName("body").style` — collection not element | Added `[0]` index |
| `getElementById("*")` — no element has id `"*"` | Line removed |
| `brightness(0.5)` — function call on undefined | Line removed |

---

### ✅ Fixed — Duplicate Variable Detection

**File:** `logic/symtab.c`

`add_symbol()` now iterates the table before inserting. If the variable name already exists it prints an error to `stderr` and returns `-1` without creating a duplicate entry. Return type changed from `void` → `int`.

```
Input:  VAR x AS INT  (twice)
Before: int x; int x;  ← silently generated
After:  Error: Variable 'x' already declared.
        int x;         ← only one declaration emitted
```

---

### ✅ Fixed — Symbol Table Overflow

**File:** `logic/symtab.c`

Previously the check `if (symbol_count < 100)` silently dropped variables beyond the cap with no diagnostic. Now:
- Capacity constant extracted to `#define MAX_SYMBOLS 100`.
- When full, `fprintf(stderr, ...)` prints a clear error and returns `-2`.
- Internal array marked `static` to prevent external mutation.

---

### ✅ Fixed — Undeclared Variable Not Caught

**File:** `logic/codegen.c`

`codegen.c` now includes `symtab.h` and calls `get_type_sym()` at every point an identifier is consumed. If the symbol is not in the table a `Warning:` is printed to `stderr`:

- `generate_expr()` — `NODE_ID` reference in any expression
- `generate_code()` — `NODE_ASSIGN` (target variable)
- `generate_code()` — `NODE_PRINT` (`SHOW x`)
- `generate_code()` — `NODE_INPUT` (`GET x`)

```
Input:  SET y TO 5  (no VAR y AS INT)
Output: Warning: Assignment to undeclared variable 'y'.
        y = 5;      ← C code still emitted, warning goes to stderr
```

---

### ✅ Fixed — AST Memory Never Freed

**Files:** `logic/ast.h`, `logic/ast.c`, `logic/main.c`

Added `void free_ast(ASTNode* node)` which recursively traverses all pointer fields (`left`, `right`, `next`, `cond`, `then_branch`, `else_branch`) and frees each node and its `str_val` string. Called in `main.c` immediately after `generate_code()` completes.

---

### ✅ Fixed — Codegen Double-Indentation Bug

**File:** `logic/main.c`

Previously `generate_code(ast_root, 1)` caused `NODE_PROGRAM` to emit children at `indent_level + 1 = 2` (double-indented). Fixed by calling `generate_code(ast_root, 0)` — the program node now passes level `1` to its children, which is correct.

```c
// Before                    // After
int main() {                 int main() {
        int x;               // double indent     int x;
        x = 1;                                   x = 1;
```

---

### ⚠️ Open — CORS Wildcard

**File:** `server/index.js`

`cors()` with no options defaults to `Access-Control-Allow-Origin: *`. Left open intentionally as this server is local-only. To lock it down, replace with:

```js
server.use(cors({ origin: 'null' }))  // for file:// opened HTML
// or
server.use(cors({ origin: 'http://localhost:5500' }))  // if using Live Server
```




## Limitations

- **Only `INT` type supported** — no `float`, `char`, `string` variables.
- **No function definitions** — only a single `main()` block.
- **No arrays or pointers.**
- **No `FOR` loop** — only `WHILE`.
- **No logical operators** (`AND`, `OR`, `NOT`) in conditions.
- **Windows + WSL required** — the server hardcodes a WSL invocation; it will not work on Linux/macOS without modification.
- **Single-user only** — shared temp files mean the app is not safe for concurrent users.
- **No syntax highlighting** in the textareas.