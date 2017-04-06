theUILang.fManager 		= "File Manager";
theUILang.fTime		= 'Time';
theUILang.fType 		= "Type";
theUILang.fPerm		= 'Permissions';
theUILang.fOpen		= 'Open';
theUILang.fCopy		= 'Copy...';
theUILang.fMove		= 'Move...';
theUILang.fRename		= 'Rename...';
theUILang.fDelete		= 'Delete';
theUILang.fExtracta		= 'Extract all...';
theUILang.fExtracth		= 'Extract here';
theUILang.fcheckSFV		= 'Check SFV';
theUILang.fMediaI		= 'Media info';
theUILang.fView		= 'View';
theUILang.fcreate		= 'Create';
theUILang.fcNewDir		= 'New Directory';
theUILang.fcNewArchive		= 'New Archive';
theUILang.fcNewTor		= 'New .torrent';
theUILang.fcSFV		= 'SFV file...';
theUILang.fcScreens		= 'Screenshots';
theUILang.fRefresh		= 'Refresh';

theUILang.fErrMsg = [];
theUILang.fErrMsg[1]		= 'Failed to change directory';
theUILang.fErrMsg[2]		= 'Directory does not exist';
theUILang.fErrMsg[3]		= 'Permission Denied';
theUILang.fErrMsg[4]		= 'Directory creation failed';
theUILang.fErrMsg[5]		= 'File is too large to handle';
theUILang.fErrMsg[6]		= 'No such file/dir';
theUILang.fErrMsg[7]		= 'Rename failed - already exists';
theUILang.fErrMsg[8]		= 'Rename failed - Denied';
theUILang.fErrMsg[9]		= 'Directory listing timed out';
theUILang.fErrMsg[10]	= 'Failed to get directory contents';
theUILang.fErrMsg[11]	= 'Action timed out';
theUILang.fErrMsg[12]	= 'Something went wrong';
theUILang.fErrMsg[13]	= 'File access error';
theUILang.fErrMsg[14]	= 'Mediainfo error';
theUILang.fErrMsg[15]	= 'Mediainfo binary not found, see config.php';
theUILang.fErrMsg[16]	= 'File already exists';
theUILang.fErrMsg[17]	= 'Temporary dir is not set, see config.php';
theUILang.fErrMsg[18]	= 'Invalid file';
theUILang.fErrMsg[19]	= 'Command finished already';
theUILang.fErrMsg[20]	= 'Command kill failed';
theUILang.fErrMsg[21]	= 'Action has finished';
theUILang.fErrMsg[22]	= 'Nothing to work with';
theUILang.fErrMsg[23]	= 'Action failed';
theUILang.fErrMsg[24]	= 'archive manipulation disabled (application not found)';




theUILang.fDiagmkdir		= 'Create new directory';
theUILang.fDiagndirname	= 'Directory name';
theUILang.fDiagnodirname 	= 'Please enter a directory name!';
theUILang.fDiagRename 	= 'Rename file / directory';
theUILang.fDiagRenameTo 	= 'Rename to:';
theUILang.fDiagRenameBut	= 'Rename';
theUILang.fDiagRenameEmpty	= 'Enter a new name';
theUILang.fDiagInvalidname	= 'Invalid name';

theUILang.fDiagAexist	= 'File / directory already exists';

theUILang.fDiagStart		= 'Start';
theUILang.fDiagStop		= 'Stop';
theUILang.fDiagClose		= 'Close';
theUILang.fDiagTo		= 'To:';
theUILang.fDiagNoPath	= 'Destination not permitted';


theUILang.fDiagDelete	= 'Delete files and folders';
theUILang.fDiagDeleteSel	= 'Files marked for removal:';
theUILang.fDiagDeleteBut	= 'Delete';

theUILang.fDiagConsole	= 'Console';

theUILang.fDiagMove		= 'Move files and folders';
theUILang.fDiagMoveSel	= 'Files marked for relocation:';
theUILang.fDiagMoveTo	= 'Move files to:';
theUILang.fDiagMoveEmpty	= 'Please select the path where to move the files to';
theUILang.fDiagMoveFalse	= 'Path not permitted';

theUILang.fDiagCopy		= 'Copy files and folders';
theUILang.fDiagCopySel	= 'Files marked for duplication:';
theUILang.fDiagCopyTo	= 'Copy files to:';

theUILang.fDiagNFO		= '.NFO Viewer';
theUILang.fDiagSFVCheck	= 'Simple File Verification';
theUILang.fDiagSFVCheckf	= 'Checksum file:';

theUILang.fDiagSFVCreate	= 'Create SFV file';
theUILang.fDiagSFVCreateSel	= 'Files marked for hashing:';
theUILang.fDiagSFVHashfile	= 'Save hash file to:';

theUILang.fDiagSFVempty	= 'Please name a sfv file';

theUILang.fDiagCArchive	= 'Create Archive';
theUILang.fDiagCArchType	= 'Type:';
theUILang.fDiagCArchVsize	= 'Volume size (mb):';

theUILang.fManArComp = []; 

// rar
theUILang.fManArComp[0] = ['Store', 'Fastest', 'Fast', 'Normal', 'Good', 'Best']

// zip
theUILang.fManArComp[1] = ['Store', 'Fast', 'Better'];

// tar
theUILang.fManArComp[2]	= theUILang.fManArComp[3] = theUILang.fManArComp[4] = ['Default'];

theUILang.fDiagCArchiveSel	= 'Add to archive:';
theUILang.fDiagExtract	= 'Extract Archive';
theUILang.fDiagArchive	= 'Archive:';
theUILang.fDiagCompression	= 'Compression:';
theUILang.fDiagExtDo		= 'Extract';
theUILang.fDiagArchempty	= 'Please name an archive file';

theUILang.fStarts = { 
	Delete: 	'File removal started',
	Extract:	'Archive extraction started',
	CArchive: 	'File archiving started',
	CheckSFV: 	'File checking started',
	CreateSFV:	'File hashing started',
	Move:		'File relocation started',
	Copy:		'File duplication started',
	Screenshots:	'Video screenshots creation started. Please wait this could take some time'
}

theUILang.fStops = { 
	Delete: 	'File removal stopped',
	Extract:	'Extraction stopped',
	CArchive: 	'Archive creation stopped',
	CheckSFV: 	'SFV check stopped',
	CreateSFV:	'SFV creation stopped',
	Move:		'File relocation stopped',
	Copy:		'File duplication stopped',
	Screenshots:		'Video screenshots creation stopped'
}
