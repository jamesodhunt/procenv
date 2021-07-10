#!/bin/bash
#---------------------------------------------------------------------
# Copyright (c) 2021 James O. D. Hunt <jamesodhunt@gmail.com>.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Description: Hacky script to "sniff" for missing symbols that
#   procenv should be reporting. It does this by searching through
#   the appropriate manpage(s) and include file(s) looking for
#   (new) symbols.
#---------------------------------------------------------------------

set -o nounset
set -o pipefail
set -e

export LC_ALL=C
export LANG=C

# XXX: Slight hack to avoid hyphenation of symbols in man-pages which
# result in the get_*() functions finding invalid symbols.
export MANWIDTH=10000

verbose=0

[ -n "${DEBUG:-}" ] && set -o xtrace

# Strip unicode crud
export man='man -E ascii'

die()
{
    local msg="$*"

    echo -e >&2 "ERROR: $msg"
    exit 1
}

info()
{
    local msg="$*"

    echo -e "INFO: $msg"
}

get_caps()
{
    local file="/usr/include/linux/capability.h"
    local manpage='capabilities(7)'

    local man_symbols
    local include_symbols
    local symbols

    [ -e "$file" ] || die "file does not exist: '$file'"
    $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"

    man_symbols=$($man "$manpage" 2>/dev/null |\
        egrep -o "\<CAP_[A-Z0-9_][A-Z0-9_]*\>" |\
        sort -u)

    include_symbols=$(egrep "#[ 	]*define[ 	]*\<CAP_" \
        "$file" |\
       grep -v '(' |\
       awk '{print $2}' |\
       sort -u)

    # Merge
    # Note: See $manpage for details of "CAP_INIT_EFF_SET"
    symbols=$(echo "$man_symbols" "$include_symbols" |\
        tr ' ' '\n' |\
        egrep -v "\<CAP_INIT_EFF_SET\>" |\
        sort -u)

    echo "$symbols"
}

get_clocks()
{
    local file="/usr/include/linux/time.h"
    local manpage='clock_gettime(2)'

    local man_symbols
    local include_symbols
    local symbols

    [ -e "$file" ] || die "file does not exist: '$file'"
    $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"

    man_symbols=$($man "$manpage" 2>/dev/null |\
        egrep -o "\<CLOCK_[^ ][^ ]*" |\
        grep -v '(' |\
        sed 's/[:,"\.-]//g' |\
        sort -u)

    include_symbols=$(egrep "#[ 	]*define[ 	]*\<CLOCK_" \
        "$file" |\
        awk '{print $2}' |\
        sort -u)

    # Merge
	#
    # Note: CLOCK_SGI_CYCLE was removed according to "$file"
    symbols=$(echo "$man_symbols" "$include_symbols" |\
        tr ' ' '\n' |\
        egrep -v "\<CLOCK_SGI_CYCLE\>" |\
        sort -u)

    echo "$symbols"
}

get_confstr()
{
    local file="/usr/include/bits/confname.h"

    local confstr_manpage='confstr(3)'
    local pathconf_manpage='pathconf(3)'
    local sysconf_manpage='sysconf(3)'

    local pathconf_man_symbols
    local sysconf_man_symbols
    local confstr_man_symbols

    local include_symbols
    local symbols

    [ -e "$file" ] || die "file does not exist: '$file'"

    local manpage

    for manpage in "$pathconf_manpage" "$confstr_manpage" "$sysconf_manpage"
    do
        $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"
    done

    pathconf_man_symbols=$($man "$pathconf_manpage" 2>/dev/null |\
        egrep -o "\<_PC_[A-Z0-9_][A-Z0-9_]*" |\
        sort -u)

    confstr_man_symbols=$($man "$pathconf_manpage" 2>/dev/null |\
        egrep -o "\<_CS_[A-Z0-9_][A-Z0-9_]*" |\
        sort -u)

    sysconf_man_symbols=$($man "$pathconf_manpage" 2>/dev/null |\
        egrep -o "\<_SC_[A-Z0-9_][A-Z0-9_]*" |\
        sort -u)

    include_symbols=$(egrep "#[ 	]*define[ 	]*\<(_CS_|_SC_|_PC_)[A-Z0-9][A-Z0-9]*" \
        "$file" |\
        awk '{print $2}' |\
        sort -u)

    # Merge
    symbols=$(echo \
        "$include_symbols" \
        "$pathconf_man_symbols" \
        "$sysconf_man_symbols" \
        "$confstr_man_symbols" |\
        tr ' ' '\n' |\
        sort -u)

    echo "$symbols"
}

get_limits()
{
    local file="/usr/include/bits/resource.h"
    local manpage='getrlimit(2)'

    local man_symbols
    local include_symbols
    local symbols

    [ -e "$file" ] || die "file does not exist: '$file'"
    $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"

    man_symbols=$($man "$manpage" 2>/dev/null |\
        egrep -o "\<RLIMIT_[A-Z_][A-Z_]*" |\
        sort -u)

    include_symbols=$($man "$manpage" 2>/dev/null |\
        egrep "#[    ]*define[       ]*\<RLIMIT_" \
        "$file" |\
        awk '{print $2}' |\
        sort -u)

    # Merge
	#
    # Note: See $manpage for details of RLIMIT_OFILE
    symbols=$(echo "$man_symbols" "$include_symbols" |\
        tr ' ' '\n' |\
        egrep -v "\<RLIMIT_NLIMITS\>" |\
        egrep -v "\<RLIMIT_OFILE\>" |\
        sort -u)

    echo "$symbols"
}

get_locale()
{
    local file="/usr/include/locale.h"
    local manpage='locale(7)'

    local man_symbols
    local include_symbols
    local symbols
    local symbol

    [ -e "$file" ] || die "file does not exist: '$file'"
    $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"

    man_symbols=$($man "$manpage" 2>/dev/null |\
        egrep -o "\<LC_[A-Z0-9_][A-Z0-9_]*" |\
        sort -u)

    include_symbols=$(egrep "#[      ]*define[       ]*\<LC_" \
        "$file" |\
        grep -v '(' |\
        awk '{print $2}' |\
        sort -u)

    # Merge
    symbols=$(echo "$man_symbols" "$include_symbols" |\
        tr ' ' '\n' |\
        sort -u)

    echo "$symbols"
}

get_prctls()
{
    local file="/usr/include/linux/prctl.h"
    local manpage='prctl(2)'

    local man_symbols
    local include_symbols
    local symbols

    [ -e "$file" ] || die "file does not exist: '$file'"
    $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"

    man_symbols=$($man "$manpage" 2>/dev/null |\
        egrep "\<PR_GET_" |\
        awk '{print $1}' |\
        grep ^PR_ |\
        tr -d , |\
        sort -u)

    include_symbols=$(egrep "#[ 	]*define[ 	]*\<PR_GET" \
        "$file" |\
        cut -d: -f2- |\
        awk '{print $2}' |\
        sort -u)

    # Merge
    symbols=$(echo "$man_symbols" "$include_symbols" |\
        tr ' ' '\n' |\
        sort -u)

    echo "$symbols"
}

get_signals()
{
    local file="/usr/include/asm/signal.h"
    local manpage='signal(7)'

    local man_symbols
    local include_symbols
    local symbols
    local symbol

    [ -e "$file" ] || die "file does not exist: '$file'"
    $man -w "$manpage" &>/dev/null || die "invalid manpage: $manpage"

    man_symbols=$($man "$manpage" 2>/dev/null |\
        egrep -o "\<SIG[A-Z0-9][A-Z0-9]*" |\
        sort -u)

    include_symbols=$(egrep "#[ 	]*define[ 	]*\<SIG[A-Z0-9][A-Z0-9]*" \
        "$file" |\
        awk '{print $2}' |\
        sort -u)

    # Merge
    symbols=$(echo "$man_symbols" "$include_symbols" |\
        tr ' ' '\n' |\
        egrep -v "\<SIGNAL\>" |\
        egrep -v "\<SIGSTKSZ\>" |\
        egrep -v "\<SIGRTMIN\>|\<SIGRTMAX\>" |\
        sort -u)

    echo "$symbols"
}

check_symbols()
{
    local name="${1:-}"
    local func="${2:-}"

    [ -z "$check" ] && die "need check name"
    [ -z "$func" ] && die "need check function"

    local symbols
    local symbol

    local source_dir='.'

    symbols=$($func)

    for symbol in $symbols
    do
        [ "$verbose" == 1 ] && info "checking $name symbol: '$symbol'"

        local found

        found=$(egrep -lr "\<${symbol}\>" \
            "${source_dir}" |\
            grep "\.[ch]$" || true)

        [ -z "$found" ] && die "$name symbol '$symbol' not referenced in source" || true
    done
}

run_checks()
{
    local -A checks=(
    [capabilities]=get_caps
    [clocks]=get_clocks
    [confstr]=get_confstr
    [limits]=get_limits
    [locale]=get_locale
    [prctl]=get_prctls
    [signals]=get_signals
    )

    local check

    local checks_sorted
    checks_sorted=$(echo "${!checks[@]}" |\
        tr ' ' '\n' |\
        sort -u |
        tr '\n' ' ')

    for check in $checks_sorted
    do
        local func="${checks[$check]}"

        info "checking '$check'"

        check_symbols "$check" "$func"
    done

    info "Checks complete"
}

handle_args()
{
    local opt

    while getopts "dv" opt "$@"
    do
        case "$opt" in
            d) set -o xtrace ;;
            v) verbose=1 ;;
        esac
    done

    shift $[$OPTIND-1]

    run_checks
}

main()
{
    handle_args "$@"
}

main "$@"
