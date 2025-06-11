#!/bin/sh

# utility script for running shell commands within a
# miniforge/conda virtual environment.
#
# %% Usage
#
# $ VENV=latest ./mini_shell.sh conda env create
#
# $ ./mini_shell.sh python -m pdb <file> ...
#
# %% The Default VENV
#
# A default VENV will be selected as the directory
# basename of the directory containing this file.
# It's assumed that this virtual environment will
# have been created such as with the following, or
# with some similar shell command.
#
# $ VENV=latest ./mini_shell.sh conda env create
#
# %% Sourcing or Running Directly
#
# For BASH and ZSH shells, this file can be sourced
# from another shell script, such as to define the
# mini_shell shell function.
#
# When not sourced from another shell script,
# then args provided to this shell script and
# received as $@ will be passed to the
# mini_shell shell function.
#
# %% Variables
#
# If miniforge  or conda is not available at
# /opt/miniforge3 then the environment variable
# MINIFORGE_PREFIX should be set to the root
# directory for the miniforge/conda installation
#
# It's assumed that the environment variable
# SHELL is defined with an absolute or relative
# pathname for a sh-like shell, e.g bash. This
# value may, but need not not denote the shell
# running this shell script.
#
# On UNIX-like hosts, the SHELL environment
# variable will typically have been set as to
# denote the user's login shell.


DEFAULT_MINIFORGE_ENV=${DEFAULT_MINIFORGE_ENV:-$(awk '$1 == "name:" { print $2; exit }' environment.yml)}

#

mini_shell() {
    local m_home="${MINIFORGE_PREFIX:-/opt/miniforge3}"
    local m_env="${VENV:-${DEFAULT_MINIFORGE_ENV}}"
    echo "using environment: ${m_env}" 1>&2
    if ! [ -e "${m_home}" ]; then
        echo "mini-shell: miniforge prefix does not exist: ${m_home}" 1>&2
        return 1
    fi
    local cmd
    if [ "$#" -eq 0 ]; then
        cmd='exec ${SHELL}'
    else
        cmd="$@"
    fi

    __CMD="${cmd}" __ENV="${m_env}" \
         MINIFORGE_PREFIX=${m_home} ${SHELL} -c '
 . ${MINIFORGE_PREFIX}/bin/activate ${__ENV} || exit;
_=$(${MINIFORGE_PREFIX}/bin/conda shell.bash hook)
unset _
. "${MINIFORGE_PREFIX}/etc/profile.d/mamba.sh";
. "${MINIFORGE_PREFIX}/etc/profile.d/conda.sh"
if ! which conda-env &>/dev/null; then
   PATH="${PATH}:${MINIFORGE_PREFIX}/bin"
fi
export CMAKE_FIND_ROOT_PATH="${CMAKE_FIND_ROOT_PATH:-}${CMAKE_FIND_ROOT_PATH:+:}${MINIFORGE_PREFIX}/envs/${__ENV}"
# echo "${PS4:-+} ${__CMD}" 1>&2
eval ${__CMD}
'; }


if [ -v BASH_SOURCE ]; then
    _SOURCE="${BASH_SOURCE}"
elif [ -v ZSH_SOURCE ]; then
    _SOURCE="${ZSH_SOURCE}"
else
    echo "$0: Unknown shell (not BASH or ZSH). Cannot detect if this file was sourced" 1>&2
    _SOURCE="<UNKNOWN>"
fi

if [ "$0" = "${_SOURCE}" ]; then
    mini_shell "$@"
fi
