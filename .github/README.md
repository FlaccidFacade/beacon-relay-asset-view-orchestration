# Organization-Wide Governance Files

This directory contains configuration files that provide automated dependency management, security, and governance across all B.R.A.V.O. repositories.

## Files Overview

### `.github/labels.yml`
**Purpose**: Defines standard labels used across all B.R.A.V.O. repositories.

**What it does**:
- Defines a comprehensive set of labels for organizing and categorizing PRs and issues
- Includes labels for:
  - **Automation tools**: copilot, dependabot
  - **Dependencies**: dependencies
  - **Programming languages**: python, javascript, typescript, java, rust, go, csharp, cpp, ruby, php, swift, kotlin
  - **Infrastructure**: terraform, docker, github-actions
  - **Change types**: documentation, ci, testing, configuration
  - **Additional categories**: bugfix, enhancement, security, performance
- Labels are automatically applied to PRs by the auto-label workflow
- Uses standard GitHub label colors matching common conventions

**How to use**:
1. Copy to any B.R.A.V.O. repository's `.github/` directory
2. Run the "Sync Labels" workflow to create/update labels in the repository
3. Customize by adding/removing labels as needed for your project
4. Labels will be automatically applied to PRs by the auto-label workflow

### `.github/workflows/auto-label-pr.yml`
**Purpose**: Automatically assigns labels to PRs based on changed files.

**What it does**:
- Triggers when a PR is opened, updated, or reopened
- Analyzes all changed files in the PR
- Automatically applies relevant labels based on:
  - **File extensions**: Detects programming languages (.py, .js, .ts, .java, .rs, .go, etc.)
  - **File names**: Identifies config files, Dockerfiles, dependency files
  - **File paths**: Recognizes test files, documentation, workflows
  - **PR metadata**: Detects Copilot PRs (branch name) and Dependabot PRs (author)
- Posts a comment listing all applied labels
- Helps with PR organization, filtering, and at-a-glance understanding

**How it works**:
1. **Checkout**: Gets the repository code
2. **Get changed files**: Compares PR branch with base branch
3. **Determine labels**: Analyzes files and determines which labels to apply
4. **Apply labels**: Uses GitHub API to add labels to the PR
5. **Comment**: Posts a summary comment (optional, fails gracefully)

**Examples**:
- PR changing `.py` files → `python` label
- PR changing `.github/workflows/*.yml` files → `github-actions` + `ci` labels
- PR from `dependabot[bot]` → `dependabot` + `dependencies` labels
- PR from `copilot/*` branch → `copilot` label
- PR changing `README.md` → `documentation` label

**How to use**:
1. Copy to any B.R.A.V.O. repository's `.github/workflows/` directory
2. Ensure labels are created in the repository (use sync-labels workflow)
3. Workflow runs automatically on all PRs
4. No configuration needed - works out of the box

### `.github/workflows/sync-labels.yml`
**Purpose**: Syncs label definitions from `.github/labels.yml` to the repository.

**What it does**:
- Triggers automatically when `.github/labels.yml` is modified on main/master branch
- Can also be triggered manually via workflow dispatch
- Creates new labels that don't exist
- Updates existing labels to match the configuration (name, color, description)
- Does not delete labels that aren't in the config file (safe operation)

**How it works**:
1. Uses the `EndBug/label-sync` action to sync labels
2. Reads configuration from `.github/labels.yml`
3. Applies changes to the repository via GitHub API

**How to use**:
1. Copy to any B.R.A.V.O. repository's `.github/workflows/` directory
2. Push changes to `.github/labels.yml` to trigger automatic sync
3. Or manually trigger via Actions tab > Sync Labels > Run workflow
4. Verify labels in repository Settings > Labels

### `.github/dependabot.yml`
**Purpose**: Automated dependency updates across all supported package ecosystems.

**What it does**:
- Configures Dependabot to check for dependency updates daily at 3 AM ET
- Supports all major package ecosystems used in B.R.A.V.O. projects:
  - **github-actions**: CI/CD workflow dependencies
  - **npm**: Web dashboard (React/Next.js)
  - **pip**: Python Lambda functions and scripts
  - **docker**: Container images
  - **maven** & **gradle**: Java/Android mobile app
  - **terraform**: Infrastructure as Code (AWS)
  - **composer**: PHP dependencies (if any)
  - **nuget**: .NET dependencies (if any)
  - **cargo**: Rust firmware dependencies (if any)
- Groups related updates into single PRs to reduce notification noise
- Automatically labels PRs by ecosystem for easy filtering
- Limits to 5 open PRs per ecosystem to prevent overwhelming maintainers

**How to use**:
1. Copy to any B.R.A.V.O. repository's `.github/` directory
2. Adjust `directory` paths if dependencies are in subdirectories
3. Customize `open-pull-requests-limit` based on repository activity

### `.github/workflows/auto-merge-dependabot.yml`
**Purpose**: Automatically approve and merge Dependabot PRs when safe.

**What it does**:
- Detects when Dependabot opens a PR
- Automatically approves the PR (counts as one required review)
- Enables GitHub's auto-merge feature
- PR merges automatically once ALL conditions are met:
  - All required status checks pass (build, test, lint)
  - PR is approved
  - Branch is up to date with base branch
  - No merge conflicts exist
- Adds a comment explaining the auto-merge status

**How it works**:
1. **Approve Job**: Runs immediately when Dependabot opens a PR
2. **Auto-merge Job**: Enables auto-merge (PR will merge once checks pass)
3. **Feedback Job**: Posts a comment explaining the process

**Safety guarantees**:
- Never merges if tests fail
- Never merges if there are conflicts
- Respects all branch protection rules
- Works seamlessly with the ruleset.json requirements

**How to use**:
1. Copy to any B.R.A.V.O. repository's `.github/workflows/` directory
2. Ensure required status checks are configured (build, test, lint)
3. Grant workflow permissions in Settings > Actions > General > Workflow permissions

### `.github/workflows/auto-delete-branch.yml`
**Purpose**: Automatically delete branches after a pull request is merged.

**What it does**:
- Triggers when a PR is closed
- Checks if the PR was actually merged (not just closed)
- Automatically deletes the source branch from the repository
- Only deletes branches from the same repository (not from forks)
- Adds a comment confirming the branch deletion

**How it works**:
1. **Delete Branch Job**: Runs when a PR is merged
2. Uses GitHub API to delete the branch
3. Posts a comment confirming the cleanup action

**Benefits**:
- Keeps the repository clean and organized
- Prevents accumulation of stale branches
- Reduces confusion about which branches are active
- Automatic cleanup without manual intervention

**Safety guarantees**:
- Only deletes after successful merge
- Never deletes branches from closed (unmerged) PRs
- Preserves fork branches (only deletes from same repository)
- Fails gracefully if branch is already deleted

**How to use**:
1. Copy to any B.R.A.V.O. repository's `.github/workflows/` directory
2. Grant workflow permissions in Settings > Actions > General > Workflow permissions
3. Works automatically without any additional configuration

### `.github/workflows/auto-label-pr.yml`
**Purpose**: Automatically assign labels to pull requests based on file changes.

**What it does**:
- Triggers when a PR is opened, synchronized, or reopened
- Analyzes changed files to detect programming languages, tools, and change types
- Automatically applies relevant labels to the PR
- Posts a comment summarizing the applied labels

**Labels applied based on**:
- **Programming languages**: Python, JavaScript, TypeScript, Java, Rust, Go, C#, C++, Ruby, PHP, Swift, Kotlin
- **Infrastructure tools**: Docker, Terraform, GitHub Actions
- **Change types**: Documentation, Testing, Configuration, Dependencies
- **Special markers**: Copilot PRs (by branch name), Dependabot PRs

**How it works**:
1. **Get changed files**: Compares PR branch with base branch
2. **Determine labels**: Analyzes file extensions and patterns
3. **Apply labels**: Uses GitHub API to add labels to the PR
4. **Comment on PR**: Posts a summary of applied labels

**Benefits**:
- Instant visual categorization of PR content
- Easy filtering and searching of PRs by type
- Better understanding of PR scope at a glance
- Automatic organization without manual effort

**How to use**:
1. Ensure labels exist in the repository (use sync-labels.yml workflow)
2. Copy to any B.R.A.V.O. repository's `.github/workflows/` directory
3. Grant workflow permissions: pull-requests: write, contents: read

### `.github/workflows/sync-labels.yml`
**Purpose**: Synchronize repository labels from the labels.yml configuration file.

**What it does**:
- Triggers when labels.yml is modified and pushed to main
- Can be manually triggered via workflow_dispatch
- Creates or updates labels in the repository based on the configuration
- Ensures consistent labeling across all repositories

**How it works**:
1. Reads label definitions from `.github/labels.yml`
2. For each label, creates it if new or updates it if existing
3. Uses GitHub API to manage labels

**Benefits**:
- Centralized label management
- Consistent labels across repositories
- Easy to add or modify labels
- Automatic synchronization

**How to use**:
1. Define labels in `.github/labels.yml`
2. Copy workflow to `.github/workflows/` directory
3. Grant workflow permissions: issues: write
4. Push changes to main or trigger manually

### `.github/labels.yml`
**Purpose**: Define standard labels used across B.R.A.V.O. repositories.

**Contains**:
- 26+ predefined labels for common use cases
- Language labels with appropriate colors
- Tool and infrastructure labels
- Change type labels (bugfix, enhancement, security, performance)

**Label categories**:
1. **Automation**: copilot, dependabot
2. **Languages**: python, javascript, typescript, java, rust, go, csharp, cpp, ruby, php, swift, kotlin
3. **Infrastructure**: terraform, docker, github-actions
4. **Change types**: documentation, ci, testing, configuration, dependencies
5. **Additional**: bugfix, enhancement, security, performance

**How to use**:
1. Copy to repository's `.github/` directory
2. Customize colors and descriptions as needed
3. Use with sync-labels.yml workflow to apply to repository

### `ruleset.json`
**Purpose**: Defines repository protection rules and governance policies.

**What it enforces**:
1. **Pull Request Requirements**:
   - At least 1 approving review required
   - Code owner review required (if CODEOWNERS file exists)
   - Stale reviews dismissed when new commits are pushed
   - All review threads must be resolved before merging

2. **Required Status Checks**:
   - `build`: Code must compile/build successfully
   - `test`: All tests must pass
   - `lint`: Code style and quality checks must pass
   - Checks must pass on the most recent commit (strict mode)

3. **Protection Against Force Pushes**:
   - Non-fast-forward rule prevents rewriting history
   - Maintains clean, auditable git history

4. **Additional Security**:
   - Required commit signatures (GPG/SSH)
   - Branch deletion protection for main/master/develop
   - Linear history requirement (no merge commits)

5. **Bypass Actors**:
   - Repository admins can bypass rules in emergencies

**Applies to branches**:
- `main`
- `master`
- `develop`

**How to apply**:
1. Go to Repository Settings > Rules > Rulesets
2. Click "New ruleset" > "Import ruleset"
3. Upload this `ruleset.json` file
4. Review and activate

Alternatively, use the GitHub API:
```bash
curl -X POST \
  -H "Authorization: token YOUR_TOKEN" \
  -H "Accept: application/vnd.github+json" \
  https://api.github.com/repos/OWNER/REPO/rulesets \
  -d @ruleset.json
```

## Integration: How These Files Work Together

```
┌─────────────────────────────────────────────────────────┐
│                    Developer Workflow                    │
└─────────────────────────────────────────────────────────┘
                            │
                            ▼
                ┌──────────────────────┐
                │   Code Changes or    │
                │  Dependabot Update   │
                └──────────────────────┘
                            │
                            ▼
                ┌──────────────────────┐
                │   Pull Request (PR)  │
                │   opened on branch   │
                └──────────────────────┘
                            │
        ┌───────────────────┼───────────────────┬──────────────────┐
        ▼                   ▼                   ▼                  ▼
┌──────────────┐  ┌──────────────────┐  ┌─────────────┐  ┌──────────────┐
│ Auto-label   │  │ Dependabot?      │  │  Ruleset    │  │  CI/CD       │
│ Analyzes     │  │ Auto-approve     │  │  Validates: │  │  Workflows   │
│ files &      │  │ & enable         │  │  - Reviews  │  │  Run:        │
│ applies      │  │ auto-merge       │  │  - Checks   │  │  - build     │
│ labels 🏷️    │  └──────────────────┘  │  - History  │  │  - test      │
└──────────────┘           │             └─────────────┘  │  - lint      │
        │                  │                    │          └─────────────┘
        │                  │                    │                  │
        └──────────────────┼────────────────────┼──────────────────┘
                           │                    │
                    ┌──────▼────────┐          │
                    │  All checks ✓ │◀─────────┘
                    │  - Tests pass │
                    │  - Approved   │
                    │  - Labeled    │
                    │  - No issues  │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │  Auto-merge   │
                    │  (Dependabot) │
                    │  or Manual    │
                    │  merge (Dev)  │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────┐
                    │  Auto-delete  │
                    │  branch after │
                    │  merge ✓      │
                    └───────────────┘
```

## Setup Checklist for New B.R.A.V.O. Repositories

- [ ] Copy `.github/labels.yml` to repository
- [ ] Copy `.github/dependabot.yml` to repository
- [ ] Copy `.github/workflows/sync-labels.yml` to repository
- [ ] Copy `.github/workflows/auto-label-pr.yml` to repository
- [ ] Copy `.github/workflows/auto-merge-dependabot.yml` to repository
- [ ] Copy `.github/workflows/auto-delete-branch.yml` to repository
- [ ] Apply `ruleset.json` via Repository Settings or API
- [ ] Run "Sync Labels" workflow to create repository labels
- [ ] Create a `CODEOWNERS` file defining code owners
- [ ] Configure required workflows: build, test, lint
- [ ] Grant workflow permissions: Settings > Actions > Workflow permissions > Read/Write
- [ ] Enable "Allow auto-merge" in Settings > General
- [ ] Test with a test Dependabot PR
- [ ] Test auto-labeling by creating a test PR

## Benefits

1. **Security**: Dependencies stay up-to-date with security patches
2. **Automation**: Less manual work for maintainers
3. **Consistency**: Same standards across all B.R.A.V.O. repositories
4. **Quality**: Required checks prevent broken code from merging
5. **Transparency**: Clear rules and automated feedback
6. **Scalability**: Works across firmware, mobile, web, API, and infrastructure repos
7. **Cleanliness**: Automatic branch cleanup keeps repositories organized
8. **Organization**: Automatic PR labeling improves discoverability and filtering
9. **Visibility**: Labels provide at-a-glance understanding of PR scope and technology

## Customization

Each repository may need slight adjustments:

- **Firmware**: May need additional checks for embedded toolchains
- **Mobile**: May need Android/iOS specific linters
- **Web**: May need bundle size checks
- **API**: May need security scanning
- **Infrastructure**: May need Terraform plan validation

Add these as additional required status checks in the ruleset or as separate jobs in the auto-merge workflow.

## Troubleshooting

**Dependabot PRs not auto-merging?**
- Check that required status checks are configured and passing
- Verify workflow permissions are set to Read/Write
- Ensure "Allow auto-merge" is enabled in repository settings
- Check branch protection rules don't conflict with auto-merge

**Ruleset not applying?**
- Verify branch names match exactly (refs/heads/main)
- Check that you have admin permissions
- Ensure enforcement is set to "active" not "evaluate"

**Too many Dependabot PRs?**
- Reduce `open-pull-requests-limit` in dependabot.yml
- Adjust grouping rules to combine more updates
- Change schedule from "daily" to "weekly"

## Contributing

To improve these organization-wide files:
1. Open an issue in this repository describing the enhancement
2. Submit a PR with changes
3. Ensure changes maintain backward compatibility
4. Update this README with any new features

## License

These configuration files are part of the B.R.A.V.O. organization open-source ecosystem.
