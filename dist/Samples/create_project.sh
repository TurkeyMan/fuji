echo "Select the type of project you would like to create:"
echo "1. GNU Makefile"
echo "2. MonoDevelop (Mono-D)"
read -p "[1-2] " -n 1 -r
echo
if [[ $REPLY =~ "1" ]]
then
	echo "Creating GNU Makefile..."
	../bin/premake5 gmake
elif [[ $REPLY =~ "2" ]]
then
	echo "Creating MonoDevelop project..."
	../bin/premake5 monodevelop
else
	echo "Invalid input: "$REPLY
fi

