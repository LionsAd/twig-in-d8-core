#!/bin/bash

FILE=$1
STATUS_S=$2
BRANCH=$(echo $(basename $FILE) | cut -d'.' -f1)

PATCH=$(grep 'href=' "$FILE" | grep files | sed 's/href="/|/g' | cut -d '|' -f2 | egrep '\.patch"|\.diff"' | cut -d'"' -f1 | tail -n 1)
TITLE=$(grep '<title>' "$FILE" | cut -d'>' -f2 | cut -d'|' -f1 | sed 's/ *$//g')
STATUS=$(grep 'Status:' "$FILE" | grep Status | head -n1 | cut -d'>' -f5 | cut -d '<' -f1)

case $TITLE in *"[meta]"*) exit 1 ;; esac
case $STATUS in "$STATUS_S"*) true ;; *) exit 1 ;; esac

echo "${TITLE}."
