# Read command line options
ARGUMENT_LIST=(
  "src_a_ip"
  "src_a_port"
  "src_a_user"
  "src_a_password"
  "src_a_gtid"
  "src_b_ip"
  "src_b_port"
  "src_b_user"
  "src_b_password"
  "src_b_gtid"
  "dst_ip"
  "dst_port"
  "dst_user"
  "dst_password"
)

# read arguments
opts=$(
  getopt \
    --longoptions "$(printf "%s:," "${ARGUMENT_LIST[@]}")" \
    --name "$(basename "$0")" \
    --options "" \
    -- "$@"
)

eval set --$opts

while true; do
  case "$1" in
  --src_a_ip)
    shift
    src_a_ip=$1
    ;;
  --src_a_port)
    shift
    src_a_port=$1
    ;;
  --src_a_user)
    shift
    src_a_user=$1
    ;;
  --src_a_password)
    shift
    src_a_password=$1
    ;;
  --src_a_gtid)
    shift
    src_a_gtid=$1
    ;;
  --src_b_ip)
    shift
    src_b_ip=$1
    ;;
  --src_b_port)
    shift
    src_b_port=$1
    ;;
  --src_b_user)
    shift
    src_b_user=$1
    ;;
  --src_b_password)
    shift
    src_b_password=$1
    ;;
  --src_b_gtid)
    shift
    src_b_gtid=$1
    ;;
  --dst_ip)
    shift
    dst_ip=$1
    ;;
  --dst_port)
    shift
    dst_port=$1
    ;;
  --dst_user)
    shift
    dst_user=$1
    ;;
  --dst_password)
    shift
    dst_password=$1
    ;;

  --)
    shift
    break
    ;;
  esac
  shift
done

cd SOURCE/build

./run \
  $src_a_ip \
  $src_a_port \
  $src_a_user \
  $src_a_password \
  $src_a_gtid \
  $src_b_ip \
  $src_b_port \
  $src_b_user \
  $src_b_password \
  $src_b_gtid \
  $dst_ip \
  $dst_port \
  $dst_user \
  $dst_password

echo RETURN VALUE IS $?
