# 🌟 Mini C-like Compiler using `win_flex` and `win_bison`

This project is a simple compiler for a **C-like language** written using `win_flex` (lex) and `win_bison` (yacc) for **Windows**. It generates:

- ✅ **Token Stream** (`tokens.txt`)
- ✅ **Three Address Code** (`tac.txt`)
- ✅ **Variable Declarations** (`decl.txt`)
- ✅ **Parse Tree (text)** (`parsetree.txt`)
- ✅ **Parse Tree (Graphviz .dot and PNG)** (`parsetree.dot`, `parsetree.png`)

---

## 📁 Project Structure

```
├── lexer.l              # Lexical analyzer
├── parser.y             # Parser (grammar rules + semantic actions)
├── parsetree.h          # Parse tree structure and helpers
├── parsetree.c          # Parse tree implementation
├── code.c               # Main driver program
├── generate_dot.c       # DOT file generator for parse tree
├── input.txt            # Sample input source code
├── tokens.txt           # Generated tokens
├── tac.txt              # Three address code output
├── decl.txt             # Variable/function declarations
├── parsetree.txt        # Text-based parse tree
├── parsetree.dot        # Graphviz DOT file (auto-generated)
├── parsetree.png        # Visual parse tree image (auto-generated)
├── Makefile             # Build and automation commands
```

---

## ⚙️ Requirements

- [`win_flex`](https://github.com/lexxmark/winflexbison)
- [`win_bison`](https://github.com/lexxmark/winflexbison)
- [`MinGW`](https://www.mingw-w64.org/) (or `gcc`)
- [`Graphviz`](https://graphviz.org/download/) (for `.png` visualization)

Make sure `dot` is in your system path for image generation.

---

## 🧠 How It Works

1. **Lexical Analysis**:
   - `lexer.l` is compiled with `win_flex` to produce `lex.yy.c`.
   - Tokens are matched and written to `tokens.txt`.

2. **Syntax Analysis & Parse Tree**:
   - `parser.y` is compiled with `win_bison` to produce `parser.tab.c/h`.
   - If parsing is successful, it writes the parse tree structure to `parsetree.txt`.

3. **Three Address Code**:
   - Expressions are translated into three-address form in `tac.txt`.

4. **Declarations**:
   - Variables and functions are logged in `decl.txt`.

5. **Parse Tree Visualization**:
   - The tree from `parsetree.txt` is parsed into a DOT file using `generate_dot.c`.
   - The DOT file is visualized as a PNG image using Graphviz.

---

## 🛠️ Build and Run (Manual Steps)

```bash
# Generate lexer
win_flex lexer.l

# Generate parser
win_bison -d parser.y

# Compile main compiler
gcc code.c lex.yy.c parser.tab.c parsetree.c -o my_compiler.exe

# Run compiler
./my_compiler.exe
```

### Parse Tree Visualization

```bash
# Compile dot generator
gcc generate_dot.c -o generate_dot.exe

# Generate dot file
./generate_dot.exe

# Generate image (requires Graphviz)
dot -Tpng parsetree.dot -o parsetree.png
```

---

## 🧪 Sample Input (`input.txt`)

## 🧹 Clean Generated Files

```bash
del lex.yy.c parser.tab.c parser.tab.h my_compiler.exe generate_dot.exe
del tokens.txt tac.txt decl.txt parsetree.txt parsetree.dot parsetree.png
```

Or use `make clean` if Makefile is available.

---

## 📌 Notes

- Parser supports `int`, `float`, `boolean`, `string`, `return`, `if`, `else`, `while`, `main`, etc.
- Function calls like `foo()` are supported in expressions.
- Error recovery is built-in using `%error` and `yyclearin`.

