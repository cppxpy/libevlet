#!/bin/sh

# utility script for running shell commands within a
# miniforge/conda/miniconda virtual environment.
#
# % Usage
#
# Install the virtual environment:
#
# $ conda env create --file ./environment.yml
#
# Run the python shell command withiin this
# project's virtual environment:
#
# $ mini_shell.sh "python ..."
#
# Run shell commands under another virtual
# environment:
#
# $ VENV=<env_name> mini_shell.sh "python ..."
#
# Run shell commands under the default virtual
# environment of a miniconda installation
# located at /opt/miniconda
#
# $ FORGE_PREFIX=/opt/miniconda mini_shell.sh python
#
# % Default FORGE_PREFIX
#
# /opt/miniforge3
#
# % Default VENV
#
# A default VENV will be selected as the directory
# basename of the directory containing this file.
#
# % Sourcing or Running Directly
#
# For BASH and ZSH shells, this file can be sourced
# from another shell script, such as to define the
# mini_shell shell function.
#
# When not sourced from another shell script,
# then any args provided to this shell script
# will be passed to the mini_shell shell
# function.
#
# % Variables
#
# If miniforge/conda is not available at
# /opt/miniforge3 then the environment variable
# FORGE_PREFIX should be set to the root
# directory for the miniforge/conda installation
#
# It's assumed that the environment variable
# SHELL is defined with an absolute or relative
# pathname for a sh-like shell, e.g bash. This
# value may, but need not not denote the shell
# running this shell script. On UNIX-like hosts,
# the SHELL environment variable# may be set
# to denote the user's preferred shell, during
# user login.
#

mini_shell() {
    local m_home="${FORGE_PREFIXi:-/opt/miniforge3}"
    local m_env="${VENV:-$(basename $(dirname $(readlink -f "$0")))}"
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
         FORGE_PREFIX=${m_home} ${SHELL} -c '
 . ${FORGE_PREFIX}/bin/activate ${__ENV} || exit;
_=$(${FORGE_PREFIX}/bin/conda shell.bash hook)
unset _
. "${FORGE_PREFIX}/etc/profile.d/mamba.sh";
. "${FORGE_PREFIX}/etc/profile.d/conda.sh"
if ! which conda-env &>/dev/null; then
   PATH="${PATH}:${FORGE_PREFIX}/bin"
fi
export CMAKE_FIND_ROOT_PATH="${CMAKE_FIND_ROOT_PATH:-}${CMAKE_FIND_ROOT_PATH:+:}${FORGE_PREFIX}/envs/${__ENV}"
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
