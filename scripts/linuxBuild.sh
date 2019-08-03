[[ "${PWD##*/}" =~ build ]] && echo "PWD has build"

# check where we are
if [ "${PWD##*/}" = "scripts" ]; then
	tmp="$(dirname "$PWD")"
	PROJECT="${tmp##*/}"
	cd ..

elif [[ "${PWD##*/}" =~ "BUILD" ]]; then
	echo "WARNING: Expecting this was run from a build directory"
	tmp="$(dirname "$PWD")"
	echo "${tmp##*/}"
	PROJECT="${tmp##*/}"
	cd ..
else
	echo "WARNING: Expecting this was run from project directory"
	PROJECT="${PWD##*/}"
fi

buildDIR="$PWD"
buildDIR+="/BUILD"
buildDIR+="_"
buildDIR+="$PROJECT"
buildDIR+="_"
buildDIR+="linux_x86"
buildDIR+="_out"

if [ ! -d "$buildDIR" ]; then
    echo "Create $buildDIR directory"
    mkdir -p $buildDIR
fi

cd $buildDIR

cmake -D PLATFORM=LINUX_X86_GCC ..

