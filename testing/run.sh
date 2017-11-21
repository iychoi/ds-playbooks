#! /bin/bash


main()
{
  local baseDir=$(dirname $(readlink -f "$0"))

  if [ "$#" -lt 1 ]
  then
    printf 'The name of a folder holding the playbook is requred\n' >&2
    return 1
  fi

  local playbooks="$1"

  if ! [[ "$playbooks" =~ ^/ ]]
  then
    playbooks="$(pwd)"/"$playbooks"
  fi

  if ! [ -d "$playbooks" ]
  then
    printf '%s is not a directory\n' "$playbooks" >&2
    return 1
  fi

  if [ "$#" -ge 2 ]
  then
    local playbook="$2"
  else
    local playbook=main.yml
  fi

  if ! [ -f "$playbooks"/"$playbook" ]
  then
    printf 'The playbook %s/%s does not exist\n' "$playbooks" "$playbook" >&2
    return 1
  fi

  if "$baseDir"/env/controller start
  then
    docker run --interactive --rm --tty \
               --network dstesting_default --volume "$playbooks":/playbooks-under-test:ro \
               ansible-tester "$playbook"
  fi

  "$baseDir"/env/controller stop
}


main "$@"