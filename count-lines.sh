#!/bin/sh

scc \
  --by-file -f html -o target/report.html -amz \
  -M .*\\.g\\..* \
  --count-as 'i:"C Header",ipp:"C++ Header"' \
  source/wn
