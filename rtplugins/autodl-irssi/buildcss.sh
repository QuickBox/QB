#!/bin/bash

shopt -s extglob

root_dir=$(git rev-parse --show-toplevel) || eval 'echo "This script must be run from within the autodl-rutorrent repo directory." 1>&2; exit 1'
styles_dir="$root_dir/styles"
css_dir="$root_dir/css"
style_files="$styles_dir/!(base).styl"

for style in $style_files
do
	name=$(basename --suffix=.styl $style)

	stylus $style -c -m -o $css_dir/$name.min.css
done
