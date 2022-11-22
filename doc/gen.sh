#!/bin/sh

asciidoctor "user manual.adoc"
asciidoctor -b manpage "manpage.adoc"
