#!/bin/sh

BRANCH=$1
git checkout develop
git merge --no-ff -m"Merged $BRANCH into develop." "$BRANCH"

if [ $? -ne 0 ]
then
  # Fail this branch
  echo "=> ERROR: Could not merge $BRANCH."
  git reset --hard develop
fi
