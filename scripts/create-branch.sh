#!/bin/bash

FILE=$1
BRANCH=$(echo $(basename $FILE) | cut -d'.' -f1)

PATCH=$(grep 'href=' "$FILE" | grep files | sed 's/href="/|/g' | cut -d '|' -f2 | egrep '\.patch"|\.diff"' | cut -d'"' -f1 | tail -n 1)
TITLE=$(grep '<title>' "$FILE" | cut -d'>' -f2 | cut -d'|' -f1 | sed 's/ *$//g')
STATUS=$(grep 'Status:' "$FILE" | grep Status | head -n1 | cut -d'>' -f5 | cut -d '<' -f1)

case $TITLE in *"[meta]"*) exit 1 ;; esac
case $STATUS in "needs review"*|"reviewed &amp; tested"*|"needs work"*) true ;; *) exit 1 ;; esac

echo "==> Applying $TITLE with $STATUS from $FILE:"
git checkout 8.x
git checkout -b "$BRANCH" || exit 1
curl "$PATCH" | git apply --index || { echo "=> ERROR: $BRANCH -- $PATCH failed to apply. ($FILE)" 1>&3 ; git checkout 8.x; git branch -D "$BRANCH"; exit 1; }
git commit -m "Issue ${TITLE}."
git push -f origin "$BRANCH"
echo "--> Committed ${TITLE}."
