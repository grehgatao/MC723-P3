#!/bin/bash
# -------------------------------------------------------------------
# ANDRE NAKAGAKI FILLIETTAZ - RA104595
# -------------------------------------------------------------------
# Script para automação da coleta de dados
# -------------------------------------------------------------------
 

 max=0

 while read i
 do
  if [[ "$i" > "$max" ]]; then
     max="$i"
  fi
 done < cicles.out

 echo "$max" > maxcicle.txt