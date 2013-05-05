#!/bin/bash

BRANCH=$1
git fetch origin
git checkout "$BRANCH" || exit 1
git rebase origin/develop
echo "Please resolve conflicts. Then type 'exit' to continue."
bash
echo 'Now continuing rebasing and pushing to origin.'
git rebase --continue
git push -f origin "$BRANCH"
