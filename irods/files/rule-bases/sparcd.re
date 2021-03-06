# VERSION: 6
#
# These are the custom rules for the Sparc'd project

@include "sparcd-env"

_sparcd_PERM = 'own'

_sparcd_logMsg(*Msg) {
  writeLine('serverLog', 'SPARCD: *Msg');
}


_sparcd_ingest(*Uploader, *TarPath) {
  _sparcd_logMsg('ingesting *TarPath for *Uploader');

  *zoneArg = execCmdArg(ipc_ZONE);
  *adminArg = execCmdArg(sparcd_ADMIN);
  *uploaderArg = execCmdArg(*Uploader);
  *tarArg = execCmdArg(*TarPath);
  *args = "*zoneArg *adminArg *uploaderArg *tarArg";
  *status = errormsg(msiExecCmd("sparcd-ingest", *args, "null", *TarPath, "null", *out), *err);

  if (*status != 0) {
    _sparcd_logMsg(*err);

    msiGetStderrInExecCmdOut(*out, *resp);

    foreach (*err in split(*resp, '\n')) {
      _sparcd_logMsg(*err);
    }

    failmsg(*status, 'SPARCD: failed to fully ingest *TarPath');
  }
}


_sparcd_isForSparcd(*Path) =
  let *strBase = str(sparcd_BASE_COLL) in *strBase != '' && *Path like *strBase ++ '/*'


_sparcd_handle_new_object(*User, *Object) {
  if (_sparcd_isForSparcd(*Object)) {
    ipc_giveAccessObj(sparcd_ADMIN, _sparcd_PERM, *Object);

    if (*Object like regex '^' ++ str(sparcd_BASE_COLL) ++ '/[^/]*/Uploads/[^/]*\\.tar$') {
      remote(ipc_RE_HOST, '') {
        _sparcd_logMsg('scheduling ingest of *Object for *User');

        delay("<PLUSET>1s</PLUSET><EF>1s REPEAT 0 TIMES</EF>")
        {_sparcd_ingest(*User, *Object);}
      }
    }
  }
}


sparcd_acPostProcForCollCreate {
  if (_sparcd_isForSparcd($collName)) {
    ipc_giveAccessColl(sparcd_ADMIN, _sparcd_PERM, $collName);
  }
}


# Add a call to this rule from inside the acPostProcForFilePathReg PEP.
sparcd_acPostProcForFilePathReg {
  _sparcd_handle_new_object($userNameClient, $objPath);
}


sparcd_acPostProcForPut {
  _sparcd_handle_new_object($userNameClient, $objPath);
}


acSetChkFilePathPerm {
  on (str(sparcd_BASE_COLL) != ''
      && $objPath like regex '^' ++ str(sparcd_BASE_COLL) ++ '/[^/]*/Uploads/.*$') {
    msiSetChkFilePathPerm('noChkPathPerm');
  }
}
