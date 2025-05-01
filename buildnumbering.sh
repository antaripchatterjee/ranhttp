#!/bin/bash

set -e

LATEST_BUILD_NUM="$1"

# Step 1: Check if git is available
if ! command -v git &>/dev/null; then
  echo "sha1sum-0000000000000000000000000000000000000000 $LATEST_BUILD_NUM"
  exit 0
fi

# Step 2: Check if repo has at least one commit
if ! git rev-parse HEAD >/dev/null 2>&1; then
  echo "sha1sum-0000000000000000000000000000000000000000 $LATEST_BUILD_NUM"
  exit 0
fi

# Step 3: Compute workspace hash (staged + unstaged + untracked)
WORKSPACE_HASH=$(
  (git diff && git diff --cached && git ls-files --others --exclude-standard -z | xargs -0 cat 2>/dev/null) \
  | sha1sum | awk '{print $1}'
)

TAG_NAME="sha1sum-${WORKSPACE_HASH}"

# Step 4: If tag exists, return same build number
if git rev-parse -q --verify "refs/tags/${TAG_NAME}" >/dev/null; then
  echo "${TAG_NAME} ${LATEST_BUILD_NUM}"
else
  # Step 5: Tag does not exist — delete all old sha1sum-* tags
  git tag -l "sha1sum-*" | xargs -r git tag -d >/dev/null 2>&1

  NEW_BUILD_NUM=$((LATEST_BUILD_NUM + 1))
  echo "${TAG_NAME} ${NEW_BUILD_NUM}"
fi

