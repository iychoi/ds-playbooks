# IPC specific AMQP rules

# Sends a message to a given AMQP topic exchange
#
# PARAMETERS:
#   *Key - The topic of the message
#   *Msg - The message to send
#
ipc_amqpSend(*Key, *Msg) {
  *exchangeArg = execCmdArg(ipc_AMQP_EXCHANGE);
  *keyArg = execCmdArg(*Key);
  *msgArg = execCmdArg(*Msg);
  *argStr = '*exchangeArg *keyArg *msgArg';
  *status = errormsg(msiExecCmd('amqptopicsend.py', *argStr, ipc_RE_HOST, 'null', 'null', *out),
                     *errMsg);
  if (*status < 0) {
    writeLine("serverLog", "Failed to send AMQP message: *errMsg");
  }
  *status;
}
