To fix the source:<br>
Make sure you have java edition 17.0.1 you can find it on google. <br>
Open project in visual studio 2022<br>
Set the build options to Release(DLL) and x64<br>
Go to project tab at top<br>
go to dope internal properties<br>
go to "VC++ Directories"<br>
than go to include directories<br>
Click on the first one and direct it to the /dope internal/ root folder.<br>
Than click okay and apply<br>
Now go to the build button at the top and click "Build Dope Internal"<br>

Set the build options to Release(XP) and x64<br>
Now click the dope loader project on the far right side.<br>
go into "VC++ Directories" like you did previously<br>
than go to include directories<br>
Click on the first one and direct it to the /BlackBone/src/ folder.<br>
Click on the next one and direct it to the /dope loader/ root folder.<br>
click ok at the bottom<br>
now go to library directories<br>
on the first one redirect it to \dope internal\vendors\ <br>
on the second one redirect it to \zlib\ which is outside the main folder.<br>
and for the fourth option redirect it to \build\x64\Release(XP)\<br>
Than click okay and apply<br>
Now go to the build button at the top and click "Build Dope Loader"<br>


Now after all this just run the exe with minecraft open and it should inject fine. <br>
Create an issue if this doesnt work for you :D<br>
