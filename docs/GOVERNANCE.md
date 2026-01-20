# OS-C Governance

## Engineering Process
OS-C follows a strict engineering contract governed by TASKS.md. This file serves as the single source of truth for project status, decisions, and roadmap.

## TASKS.md Contract
- **Binding**: All changes must align with TASKS.md decisions
- **Chronological**: Updates are appended chronologically
- **Accountable**: Each milestone includes what was done, issues, next steps
- **Locked**: Architecture decisions are final once documented

## Milestone Process
1. **Proposal**: New features proposed in TASKS.md
2. **Approval**: Consensus required for major changes
3. **Implementation**: Code developed in phases
4. **Testing**: Comprehensive tests added
5. **Review**: Code review and integration testing
6. **Milestone**: Update TASKS.md with completion status

## Code Standards
- **Style**: Linux kernel style (4-space indent, 80-char lines)
- **Safety**: No undefined behavior, freestanding C11
- **Documentation**: Inline comments, separate docs for subsystems
- **Testing**: 100% test coverage for new code

## Change Control
- **Commits**: One per milestone, descriptive messages
- **Branches**: Feature branches for development
- **Reviews**: All PRs require review
- **Releases**: Tagged releases with changelogs

## Quality Assurance
- **CI/CD**: Automated build + test on commits
- **Regression Testing**: No regressions allowed
- **Hardware Validation**: Tested on multiple platforms
- **Security**: Regular audits, hardening applied

## Roles
- **Architect**: Defines high-level design
- **Engineer**: Implements features
- **Reviewer**: Ensures quality
- **Maintainer**: Oversees project health

This governance ensures OS-C remains a professional, credible kernel project.