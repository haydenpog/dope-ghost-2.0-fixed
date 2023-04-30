To fix the source:
Make sure you have java edition 17.0.1 you can find it on google. \n
Open project in visual studio 2022
Set the build options to Release(DLL) and x64
Go to project tab at top
go to dope internal properties
go to "VC++ Directories"
than go to include directories
Click on the first one and direct it to the /dope internal/ root folder.
Than click okay and apply
Now go to the build button at the top and click "Build Dope Internal"

Set the build options to Release(XP) and x64
Now click the dope loader project on the far right side.
go into "VC++ Directories" like you did previously
than go to include directories
Click on the first one and direct it to the /BlackBone/src/ folder.
Click on the next one and direct it to the /dope loader/ root folder.
click ok at the bottom
now go to library directories
on the first one redirect it to \dope internal\vendors\ 
on the second one redirect it to \zlib\ which is outside the main folder.
and for the fourth option redirect it to \build\x64\Release(XP)\
Than click okay and apply
Now go to the build button at the top and click "Build Dope Loader"


Now after all this just run the exe with minecraft open and it should inject fine. 
Create an issue if this doesnt work for you :D
