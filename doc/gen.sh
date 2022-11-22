#!/bin/sh

asciidoctor "user manual.adoc"
asciidoctor -b manpage "manpage.adoc"
asciidoctor "hw.adoc"
