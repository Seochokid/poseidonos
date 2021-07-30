#!/bin/bash

source ../config.sh

target_ip_1=${TARGET_IP_1}
target_ip_2=${TARGET_IP_2}
target_nic_1=${TARGET_NIC_1}
target_nic_2=${TARGET_NIC_2}
volume_cnt=${VOLUME_CNT}
volume_gb_size=${VOLUME_GB_SIZE}
volume_byte_size=${VOLUME_BYTE_SIZE}
# second
seq_io_time=${SEQ_IO_TIME}
rand_io_time=${RAND_IO_TIME}

init1_id=${INIT_1_ID}
init2_id=${INIT_2_ID}
init1_pw=${INIT_1_PW}
init2_pw=${INIT_2_PW}
init1_ip=${INIT_1_IP}
init2_ip=${INIT_2_IP}
init1_vdbench_dir=${INIT1_VDBENCH_DIR}
init2_vdbench_dir=${INIT2_VDBENCH_DIR}
vdbench_sub_initiator_ip=${VDBENCH_SUB_INIT_IP}
init1_fio_conf_dir=${INIT1_FIO_CONF_DIR}
init2_fio_conf_dir=${INIT2_FIO_CONF_DIR}
init1_fio_engine=${INIT1_FIO_ENGINE}
init2_fio_engine=${INIT2_FIO_ENGINE}
init1_files=${INIT1_FILES}
init2_files=${INIT2_FILES}

while getopts r:s: flag
do
	case "${flag}" in
		r) rand_io_time=${OPTARG};;
		s) seq_io_time=${OPTARG};;
	esac
done

#sudo ../4_10hr_test/1_start_pos.sh
#sudo ../4_10hr_test/2_bring_up.sh -a ${target_ip_1} -b ${target_ip_2} -s ${volume_cnt} -v ${volume_cnt} -S ${volume_byte_size} -n ${target_nic_1} -m ${target_nic_2}

echo "creating fio configuration"
sudo ./create_test_config.sh -a ${target_ip_1} -b ${target_ip_2} -v ${volume_cnt} -S ${volume_gb_size} -s ${seq_io_time} -r ${rand_io_time} -p ${vdbench_sub_initiator_ip} -f ${init1_fio_conf_dir} -e ${init1_fio_engine}

sshpass -p ${init1_pw} ssh ${init1_id}@${init1_ip} "mkdir -p ${init1_fio_conf_dir}"
for file in ${init1_files}
do
    sshpass -p ${init1_pw} scp -o StrictHostKeyChecking=no ${file} ${init1_id}@${init1_ip}:${init1_fio_conf_dir}
done

sshpass -p ${init2_pw} ssh ${init2_id}@${init2_ip} "mkdir -p ${init2_fio_conf_dir}"
for file in ${init2_files}
do
    sshpass -p ${init2_pw} scp -o StrictHostKeyChecking=no ${file} ${init2_id}@${init2_ip}:${init2_fio_conf_dir}
done
echo "fio configuration created"

if [ ${rand_io_time} -gt 1 ]
then
echo "random write start"
sshpass -p ${init1_pw} ssh ${init1_id}@${init1_ip} "cd ${init1_fio_conf_dir}; echo ${init1_pw} | sudo -S nohup fio ./rw_tcp_init1.conf > /dev/null 2>&1 &"
sshpass -p ${init2_pw} ssh ${init2_id}@${init2_ip} "cd ${init2_fio_conf_dir}; echo ${init2_pw} | sudo -S nohup fio ./rw_tcp_init2.conf > /dev/null 2>&1"
echo "random write sleep"
fi


