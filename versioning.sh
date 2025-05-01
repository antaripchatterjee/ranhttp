#!/bin/bash

set -e

INITIAL_VERSION="v0.0.1"
# Check for any commits
if ! git rev-parse HEAD >/dev/null 2>&1; then
  echo "$INITIAL_VERSION"
  exit 1
fi

LATEST_VERSION_TAG=$(git tag --sort=-creatordate --list "v*" | head -n 1)

if [[ -z "$LATEST_VERSION_TAG" ]]; then
  LAST_TAG="$INITIAL_VERSION"
  COMMITS=$(git log --reverse HEAD --pretty=format:"%s%n%b")
else
  LAST_TAG="$LATEST_VERSION_TAG"
  COMMITS=$(git log --reverse "${LAST_TAG}..HEAD" --pretty=format:"%s%n%b")
fi

VERSION=${LAST_TAG#v}
IFS='.' read -r MAJOR MINOR PATCH <<< "$VERSION"

# Track version bump state
MAJOR_BUMPED=false

# Reverse iterate and handle logic
while read -r COMMIT_MSG ; do

  LOWER_MSG=$(echo "$COMMIT_MSG" | sed -E 's/^[^a-zA-Z0-9\[]+//; s/[^a-zA-Z0-9\]]+$//' | tr '[:upper:]' '[:lower:]')

  # Skip if commit is marked as no version change
  if [[ -z "$LOWER_MSG" || "$LOWER_MSG" == "merge"* \
      || "$LOWER_MSG" == "nvc:"* || "$LOWER_MSG" == "[nvc]"* \
      || "$LOWER_MSG" == "note:"* || "$LOWER_MSG" == "[note]"* \
      || "$LOWER_MSG" == "chore:"* || "$LOWER_MSG" == "[chore]"* \
      || "$LOWER_MSG" == "test:"* || "$LOWER_MSG" == "[test]"* \
      || "$LOWER_MSG" == "refactor:"* || "$LOWER_MSG" == "[refactor]"* \
      || "$LOWER_MSG" == "refact:"* || "$LOWER_MSG" == "[refact]"* \
      || "$LOWER_MSG" == "doc:"* || "$LOWER_MSG" == "[doc]"* \
      || "$LOWER_MSG" == "docs:"* || "$LOWER_MSG" == "[docs]"* \
      || "$LOWER_MSG" == "document:"* || "$LOWER_MSG" == "[document]"* \
      || "$LOWER_MSG" == "readme:"* || "$LOWER_MSG" == "[readme]"* \
      || "$LOWER_MSG" == "license:"* || "$LOWER_MSG" == "[license]"* \
      || "$LOWER_MSG" == "sub:"* || "$LOWER_MSG" == "[sub]"* \
      || "$LOWER_MSG" == "subject:"* || "$LOWER_MSG" == "[subject]"* ]]; then
    continue
  fi

  # Handle major bumps
  if [[ "$LOWER_MSG" == "!:"* || "$LOWER_MSG" == "[!]"* \
      || "$LOWER_MSG" == "fix!:"* || "$LOWER_MSG" == "[fix!]"* \
      || "$LOWER_MSG" == "major!:"* || "$LOWER_MSG" == "[major!]"* \
      || "$LOWER_MSG" == "major:"* || "$LOWER_MSG" == "[major]"* \
      || "$LOWER_MSG" == "minor!:"* || "$LOWER_MSG" == "[minor!]"* \
      || "$LOWER_MSG" == "patch!:"* || "$LOWER_MSG" == "[patch!]"* \
      || "$LOWER_MSG" == "feat!:"* || "$LOWER_MSG" == "[feat!]"* \
      || "$LOWER_MSG" == "feature!:"* || "$LOWER_MSG" == "[feature!]"* \
      || "$LOWER_MSG" == "change!:"* || "$LOWER_MSG" == "[change!]"* \
      || "$LOWER_MSG" == "change:"* || "$LOWER_MSG" == "[change]"* \
      || "$LOWER_MSG" == "break:!"* || "$LOWER_MSG" == "[break!]"* \
      || "$LOWER_MSG" == "break:"* || "$LOWER_MSG" == "[break]"* ]]; then

    if ! $MAJOR_BUMPED; then
      ((++MAJOR))
      MINOR=0
      PATCH=0
      MAJOR_BUMPED=true
    fi

  # Handle minor bumps
  elif [[ "$LOWER_MSG" == minor:* || "$LOWER_MSG" == "[minor]"* \
        || "$LOWER_MSG" == feat:* || "$LOWER_MSG" == "[feat]"* \
        || "$LOWER_MSG" == feature:* || "$LOWER_MSG" == "[feature]"* ]]; then

    ((++MINOR))
    PATCH=0

  # Handle patch bumps (default)
  else
    ((++PATCH))
  fi

done <<< $(echo "$COMMITS" | tac)

NEW_VERSION="v${MAJOR}.${MINOR}.${PATCH}"
echo "$NEW_VERSION"
if [[ "${NEW_VERSION#v}" == "$VERSION" ]]; then
  exit 1
fi

