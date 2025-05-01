#!/bin/bash

set -e

ADD_FILES=""
COMMIT_TYPE=""
COMMIT_MSG=""
USED_TYPE=""

for arg in "$@"; do
  case "$arg" in
    add=*)
      ADD_FILES="${arg#add=}"
      ;;
    *=*)
      if [[ -z "$USED_TYPE" ]]; then
        KEY=$(echo "${arg%%=*}" | tr '[:upper:]' '[:lower:]')
        MSG="${arg#*=}"

        case "$KEY" in
          m)
            COMMIT_TYPE=""
            COMMIT_MSG="$MSG"
            ;;
          nvc|note|chore|test|refactor|refact|doc|docs|document|readme|license|sub|subject|patch|fix|minor|feat|feature|change|break|major)
            COMMIT_TYPE="$KEY"
            COMMIT_MSG="$MSG"
            ;;
          defix)
            COMMIT_TYPE="fix!"
            COMMIT_MSG="$MSG"
            ;;
          depatch)
            COMMIT_TYPE="patch!"
            COMMIT_MSG="$MSG"
            ;;
          deminor)
            COMMIT_TYPE="minor!"
            COMMIT_MSG="$MSG"
            ;;
          demajor)
            COMMIT_TYPE="major!"
            COMMIT_MSG="$MSG"
            ;;
          defeat|defeatue)
            COMMIT_TYPE="feat!"
            COMMIT_MSG="$MSG"
            ;;
          dechange)
            COMMIT_TYPE="change!"
            COMMIT_MSG="$MSG"
            ;;
          debreak)
            COMMIT_TYPE="break!"
            COMMIT_MSG="$MSG"
            ;;
          *)
            echo "Unknown commit type: $KEY"
            exit 1
            ;;
        esac
        USED_TYPE="yes"
      fi
      ;;
  esac
done

if [[ -n "$ADD_FILES" ]]; then
  echo "Adding files: $ADD_FILES"
  git add $ADD_FILES
fi

if [[ -z "$COMMIT_MSG" ]]; then
  echo "Missing commit message"
  exit 1
fi

if [[ -n "$COMMIT_TYPE" ]]; then
  echo "Committing with: $COMMIT_TYPE: $COMMIT_MSG"
  git commit -m "$COMMIT_TYPE: $COMMIT_MSG"
else
  echo "Committing with: $COMMIT_MSG"
  git commit -m "$COMMIT_MSG"
fi

