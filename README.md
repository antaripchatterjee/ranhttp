
# RANHTTP

Rapid and Native HTTP Development with C language. Development is in progress.

## Description

---

## 🛠️ Project Structure

```
ranhttp (root)
├── LICENSE
├── Makefile
├── README.md
├── build
│   ├── debug
│   │   ├── bin
│   │   │   ├── ranhttp
│   │   │   └── libmymodule.dll
│   │   └── lib
│   │       ├── libmymodule.dll
│           ├── libmymodule.so
│           ├── libmymodule.a
│   │       └── libmymodule.dll.a
│   └── release
│       ├── bin
│       │   ├── ranhttp
│       │   └── libmymodule.dll
│       └── lib
│           ├── libmymodule.dll
│           ├── libmymodule.so
│           ├── libmymodule.a
│           └── libmymodule.dll.a
├── buildnumbering.sh
├── commithelper.sh
├── example
├── headers
│   ├── common.h
│   └── mymodule
│       └── main.h
├── main.cpp
├── objects
│   ├── main.cpp.o
│   ├── src
│   │   └── mymodule
│   │       └── main.cpp.o
│   └── test
├── src
│   └── mymodule
│       └── main.cpp
├── test
│   ├── results
│   │   └── test__mymodule.exe
│   └── test__mymodule.cpp
└── versioning.sh
```

---

## ⚙️ Initialization
>> Incase you already initialized the project with git during project creation, please ignore this section.

1. Make sure `git` is installed.
2. Initialize your repository:

```bash
cd "ranhttp"
git init
git commit --allow-empty -m "Initial commit"
git tag -a "v0.0.1" -m "ranhttp v0.0.1 build 0"
```

> Annotated tag `v0.0.1` is mandatory for versioning.

---

## 🔨 Build Commands

| Command          | Description                                       |
|------------------|---------------------------------------------------|
| `make debug`   | Builds the project in **debug** mode              |
| `make`         | Default build; runs `make release`              |
| `make release` | Builds the project in **release** mode            |
| `make clean`   | Cleans object files and binaries                  |

All compiled objects go under `objects/`.  
Binaries and shared libraries are stored in `build/debug` or `build/release` folders.

---

## 🧪 Testing & Mocking

| Command         | Description                                                                 |
|-----------------|-----------------------------------------------------------------------------|
| `make mocks`  | Compiles test cases in `test/`, outputs binaries in `test/results/`      |
| `make test`   | Executes test binaries and validates their exit codes (expected: `0`)     |

---

## 🧬 Version & Build Control

- Uses annotated tags like `v1.2.3` to represent semantic versions.
- Versions increment based on commit message prefixes.
- Lightweight tags like `build-42` and `sha1sum-<hash>` are used for tracking build state.
- Hash includes committed + staged + untracked file content.
- Redundant `sha1sum-*` tags are auto-cleaned when builds change.

---

## ✅ Versioning Workflow

### 🔧 Commiting with Metadata

Use:

```bash
make commit <type>="Your commit message" [add=<file>]
```

- `add` is optional
- `<type>` helps identify the change type for versioning

### Supported Types

| Type(s)                                         | Action        | Description                                 |
|-------------------------------------------------|---------------|---------------------------------------------|
| `nvc`, `note`, `chore`                    | No Version    | No version change                           |
| `test`                                        | No Version    | Change test related code                    |
| `refactor`, `refact`                        | No Version    | Code refactor only                          |
| `doc`, `docs`, `document`                 | No Version    | Documentation update                        |
| `readme`, `license`                         | No Version    | Readme or license changes                   |
| `sub`, `subject`                            | No Version    | PR merge subject                            |
| `m`                                           | Patch         | Patch change with no prefix                 |
| `fix`, `patch`                              | Patch         | Patch change                                |
| `feat`, `feature`, `minor`                | Minor         | Minor/feature addition                      |
| `change`, `break`, `major`                | Major         | Major/breaking changes                      |
| `defix`, `depatch`                          | Major         | Deprecating Change or reverting patches     |
| `deminor`, `defeat`, `defeature`          | Major         | Deprecating features                        |
| `dechange`, `debreak`, `demajor`          | Major         | Deprecating major features change           |


#### Example

```bash
make commit change="Auth process changed" add=src/auth.cpp
make commit feat="Add login" add=src/login.cpp
make commit fix="Fix crash on start"
make commit doc="Docs update"
```

---

## 🔼 Bumping Version

After committing, run:

```bash
make bump
```

- Inspects all commits since last version tag
- Applies correct version bump based on message prefixes
- Creates a new annotated version tag

> Commits are analyzed from **oldest to newest**, including squash commits.

---

## 📦 Build Numbering

- Incremental build numbers (e.g. `build-1`, `build-2`) are created only if changes are detected.
- These builds are tracked by lightweight tags.
- Workspace hash is computed to uniquely identify changes using:

```bash
git diff && git diff --cached && git ls-files --others --exclude-standard
```

---

## 🧠 Behind the Scenes

- `versioning.sh`: Determines version from commit messages
- `buildnumbering.sh`: Tracks and updates build numbers based on content hash
- `commithelper.sh`: Formats commits with prefixes and auto-handles `git add`


