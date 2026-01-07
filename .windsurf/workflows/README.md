# Corporate Game Engine Workflows

> Complete CI/CD pipeline and development workflows for 50+ developer teams

## Overview

This folder contains **production-ready workflows** for enterprise game engine development, inspired by:
- **Epic Games** (Unreal Engine)
- **Unity Technologies**
- **Valve** (Source Engine)
- **id Software** (id Tech)
- **AWS Game Tech**
- **IGDA Best Practices**

## Workflow Categories

### 🚀 Session Management
| Workflow | Description |
|----------|-------------|
| `init-session.md` | Start development session |
| `end-session.md` | End session with documentation sync |
| `handoff.md` | Team handoff between shifts/timezones |

### 🔧 Development
| Workflow | Description |
|----------|-------------|
| `new-feature.md` | Complete feature development cycle |
| `add-system.md` | Add new engine system |
| `add-component.md` | Add actor component |
| `add-asset.md` | Import and manage assets |
| `refactor.md` | Safe refactoring process |
| `hotfix.md` | Emergency production fixes |

### 🏗️ Build & Compile
| Workflow | Description |
|----------|-------------|
| `build.md` | Standard build process |
| `build-all-platforms.md` | Multi-platform builds |
| `compile.md` | Quick compilation |
| `clean-build.md` | Full rebuild from scratch |

### 🧪 Testing & QA
| Workflow | Description |
|----------|-------------|
| `run-tests.md` | Execute test suite |
| `add-test.md` | Create new tests |
| `performance-test.md` | Performance benchmarking |
| `playtest.md` | QA playtest session |

### 🔍 Code Review
| Workflow | Description |
|----------|-------------|
| `code-review.md` | Standard code review |
| `architecture-review.md` | Architecture/design review |
| `security-review.md` | Security audit |

### 📝 Git & Version Control
| Workflow | Description |
|----------|-------------|
| `git-commit.md` | Commit with conventions |
| `git-branch.md` | Branch management |
| `git-merge.md` | Merge strategies |
| `generate-pr.md` | Create pull request |
| `resolve-conflict.md` | Conflict resolution |

### 📚 Documentation
| Workflow | Description |
|----------|-------------|
| `update-docs.md` | Update documentation |
| `update-commentary.md` | Code comments update |
| `update-roadmap.md` | Roadmap progress |
| `update-changelog.md` | Release notes |
| `create-adr.md` | Architecture Decision Record |

### 🐛 Debugging & Investigation
| Workflow | Description |
|----------|-------------|
| `debug.md` | Debugging workflow |
| `investigate.md` | Bug investigation |
| `profile.md` | Performance profiling |
| `crash-analysis.md` | Crash dump analysis |

### 🚢 Release & Deployment
| Workflow | Description |
|----------|-------------|
| `release-candidate.md` | Prepare RC build |
| `release.md` | Production release |
| `deploy-staging.md` | Deploy to staging |
| `rollback.md` | Emergency rollback |

### 👥 Team & Onboarding
| Workflow | Description |
|----------|-------------|
| `onboard-developer.md` | New developer setup |
| `sprint-planning.md` | Sprint planning session |
| `retrospective.md` | Sprint retrospective |
| `incident-response.md` | Production incident handling |

## Quick Start

1. Copy desired workflows to `.windsurf/workflows/`
2. Customize paths and project-specific settings
3. Use via `/workflow-name` in your IDE

## File Structure

```
corporate-workflows/
├── README.md                    # This file
├── session/                     # Session management
├── development/                 # Feature development
├── build/                       # Build & compile
├── testing/                     # Tests & QA
├── review/                      # Code review
├── git/                         # Version control
├── docs/                        # Documentation
├── debug/                       # Debugging
├── release/                     # Deployment
└── team/                        # Team processes
```

## License

MIT - Use freely in your game engine projects.
