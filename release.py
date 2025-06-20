#!/usr/bin/env python3

"""
Release script for AlgorithmVisualizer
This script:
1. Parses conventional commits since the last tag
2. Determines the next semantic version number
3. Updates CMakeLists.txt with the new version
4. Generates a changelog entry
5. Creates a Git tag for the release
"""

import os
import re
import subprocess
import sys
from datetime import datetime

# Configuration
PROJECT_NAME = "AlgorithmVisualizer"
CMAKE_FILE = "CMakeLists.txt"
CHANGELOG_FILE = "CHANGELOG.md"

def run_command(cmd, get_output=False):
    """Execute a shell command and optionally return its output."""
    try:
        if get_output:
            result = subprocess.run(cmd, shell=True, check=True, capture_output=True, text=True)
            return result.stdout.strip()
        else:
            subprocess.run(cmd, shell=True, check=True)
            return True
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {cmd}")
        print(f"Error details: {e}")
        return False

def get_current_version():
    """Extract version from CMakeLists.txt."""
    with open(CMAKE_FILE, 'r') as f:
        content = f.read()
    
    match = re.search(r'project\([^\)]*VERSION\s+(\d+)\.(\d+)\.(\d+)', content)
    if not match:
        print("Could not find version in CMakeLists.txt")
        sys.exit(1)
    
    major = int(match.group(1))
    minor = int(match.group(2))
    patch = int(match.group(3))
    
    return (major, minor, patch)

def get_last_tag():
    """Get the most recent Git tag."""
    result = run_command("git describe --tags --abbrev=0 2>/dev/null || echo 'none'", get_output=True)
    return result

def get_commits_since_tag(tag):
    """Get conventional commits since the specified tag."""
    if tag == "none":
        # If no tag exists, get all commits
        cmd = "git log --pretty=format:'%H %s'"
    else:
        cmd = f"git log {tag}..HEAD --pretty=format:'%H %s'"
    
    commits_str = run_command(cmd, get_output=True)
    
    if not commits_str:
        return []
    
    commits = []
    for line in commits_str.split('\n'):
        if line.strip():
            commits.append(line)
    
    return commits

def analyze_commits(commits):
    """Analyze conventional commits to determine version bump."""
    has_breaking = False
    has_feature = False
    has_fix = False
    
    changes = {
        'feat': [],
        'fix': [],
        'perf': [],
        'breaking': [],
        'other': []
    }
    
    for commit in commits:
        hash_value, message = commit.split(' ', 1)
        short_hash = hash_value[:7]
        
        # Check for breaking changes
        if "BREAKING CHANGE:" in message or "!" in message.split(":")[0]:
            has_breaking = True
            changes['breaking'].append(f"{message} ({short_hash})")
            continue
        
        # Match conventional commit format
        match = re.match(r'^(\w+)(?:\(([^\)]+)\))?: (.+)', message)
        if match:
            commit_type = match.group(1)
            scope = match.group(2) or ''
            desc = match.group(3)
            
            if commit_type == 'feat':
                has_feature = True
                changes['feat'].append(f"{desc} {f'({scope}) ' if scope else ''}({short_hash})")
            elif commit_type == 'fix':
                has_fix = True
                changes['fix'].append(f"{desc} {f'({scope}) ' if scope else ''}({short_hash})")
            elif commit_type == 'perf':
                changes['perf'].append(f"{desc} {f'({scope}) ' if scope else ''}({short_hash})")
            else:
                changes['other'].append(f"{commit_type}: {desc} {f'({scope}) ' if scope else ''}({short_hash})")
        else:
            # Non-conventional commit
            changes['other'].append(f"{message} ({short_hash})")
    
    # Determine version bump
    if has_breaking:
        bump = "major"
    elif has_feature:
        bump = "minor"
    elif has_fix:
        bump = "patch"
    else:
        bump = "patch"  # Default to patch if no relevant changes
    
    return bump, changes

def update_version(current_version, bump_type):
    """Create a new version based on bump type."""
    major, minor, patch = current_version
    
    if bump_type == "major":
        return (major + 1, 0, 0)
    elif bump_type == "minor":
        return (major, minor + 1, 0)
    else:  # patch
        return (major, minor, patch + 1)

def update_cmake_version(new_version):
    """Update version in CMakeLists.txt."""
    major, minor, patch = new_version
    version_str = f"{major}.{minor}.{patch}"
    
    with open(CMAKE_FILE, 'r') as f:
        content = f.read()
    
    new_content = re.sub(
        r'project\(([^\)]*VERSION\s+)(\d+\.\d+\.\d+|\d+\.\d+)',
        f'project(\\1{version_str}',
        content
    )
    
    with open(CMAKE_FILE, 'w') as f:
        f.write(new_content)
    
    return version_str

def generate_changelog(version, changes, tag_date):
    """Generate a changelog entry for the new version."""
    version_str = f"{version}"
    header = f"## [{version_str}] - {tag_date}"
    
    sections = []
    if changes['breaking']:
        sections.append("### ⚠ BREAKING CHANGES\n\n" + "\n".join([f"* {item}" for item in changes['breaking']]))
    
    if changes['feat']:
        sections.append("### Features\n\n" + "\n".join([f"* {item}" for item in changes['feat']]))
    
    if changes['fix']:
        sections.append("### Bug Fixes\n\n" + "\n".join([f"* {item}" for item in changes['fix']]))
    
    if changes['perf']:
        sections.append("### Performance Improvements\n\n" + "\n".join([f"* {item}" for item in changes['perf']]))
    
    if changes['other']:
        sections.append("### Other Changes\n\n" + "\n".join([f"* {item}" for item in changes['other']]))
    
    changelog_content = header + "\n\n" + "\n\n".join(sections)
    
    # Create or update CHANGELOG.md
    if os.path.exists(CHANGELOG_FILE):
        with open(CHANGELOG_FILE, 'r') as f:
            existing_content = f.read()
        
        # Find the position to insert the new content
        marker = "# Changelog"
        if marker in existing_content:
            pos = existing_content.find(marker) + len(marker)
            new_content = existing_content[:pos] + "\n\n" + changelog_content + existing_content[pos:]
        else:
            new_content = f"# Changelog\n\n{changelog_content}\n\n{existing_content}"
    else:
        new_content = f"# Changelog\n\n{changelog_content}\n"
    
    with open(CHANGELOG_FILE, 'w') as f:
        f.write(new_content)
    
    return changelog_content

def create_git_tag(version):
    """Create a Git tag for the new version."""
    version_str = f"v{version}"
    run_command(f"git tag -a {version_str} -m 'Release {version_str}'")
    return version_str

def main():
    # Check if git repo has uncommitted changes
    status = run_command("git status --porcelain", get_output=True)
    if status:
        print("Error: There are uncommitted changes in the repository.")
        print("Please commit or stash your changes before running this script.")
        sys.exit(1)
    
    # Get current version
    current_version = get_current_version()
    current_version_str = '.'.join(map(str, current_version))
    print(f"Current version: {current_version_str}")
    
    # Get last tag and commits since then
    last_tag = get_last_tag()
    print(f"Last tag: {last_tag}")
    
    commits = get_commits_since_tag(last_tag)
    if not commits:
        print("No new commits since last tag. Nothing to release.")
        sys.exit(0)
    
    print(f"Found {len(commits)} commits since last tag")
    
    # Analyze commits and determine next version
    bump_type, changes = analyze_commits(commits)
    new_version = update_version(current_version, bump_type)
    new_version_str = '.'.join(map(str, new_version))
    
    print(f"Bump type: {bump_type}")
    print(f"New version: {new_version_str}")
    
    # Confirm with user
    confirm = input(f"Update version to {new_version_str}? (y/n): ")
    if confirm.lower() != 'y':
        print("Release canceled.")
        sys.exit(0)
    
    # Update CMakeLists.txt
    print("Updating CMakeLists.txt...")
    update_cmake_version(new_version)
    
    # Generate changelog
    print("Generating changelog...")
    today = datetime.now().strftime("%Y-%m-%d")
    changelog_entry = generate_changelog(new_version_str, changes, today)
    
    # Commit changes
    print("Committing changes...")
    run_command(f"git add {CMAKE_FILE} {CHANGELOG_FILE}")
    run_command(f"git commit -m 'chore(release): {new_version_str}'")
    
    # Create tag
    print("Creating Git tag...")
    tag = create_git_tag(new_version_str)
    
    print(f"""
Release {new_version_str} completed successfully!

Summary of changes:
{changelog_entry}

Next steps:
1. Push the changes: git push origin main
2. Push the tag: git push origin {tag}
""")

if __name__ == "__main__":
    main()
