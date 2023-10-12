#include <iostream>
#include <cstring>
#include <cstdint>
#include "include/livox_lidar_api.h"
#include "include/colcor_log.h"
#include <unistd.h>

std::string global_sn;
std::string global_update_ip;
uint8_t global_updata_state=0;   // 0 未修改;1 修改成功 2 修改失败 

bool isSubstring(const std::string& a, const std::string& b) {
    return a.find(b) != std::string::npos || b.find(a) != std::string::npos;
}

void RebootCallback(livox_status status, uint32_t handle, LivoxLidarRebootResponse* response, void* client_data) {
  if (response == nullptr) {
    global_updata_state = 2;
    return;
  }
  printf("RebootCallback, status:%u, handle:%u, ret_code:%u\n",
      status, handle, response->ret_code);
  global_updata_state = 1;
}

void SetIpInfoCallback(livox_status status, uint32_t handle, LivoxLidarAsyncControlResponse *response, void *client_data) {
  if (response == nullptr) {
    return;
  }
  printf("LivoxLidarIpInfoCallback, status:%u, handle:%u, ret_code:%u, error_key:%u\n",
      status, handle, response->ret_code, response->error_key);

  if (response->ret_code == 0 && response->error_key == 0) {
    LivoxLidarRequestReboot(handle, RebootCallback, nullptr);
  }
}

void WorkModeCallback(livox_status status, uint32_t handle,LivoxLidarAsyncControlResponse *response, void *client_data) {
  if (response == nullptr) {
    return;
  }
  printf("WorkModeCallack, status:%u, handle:%u, ret_code:%u, error_key:%u\n",
      status, handle, response->ret_code, response->error_key);

}

void QueryInternalInfoCallback(livox_status status, uint32_t handle, 
    LivoxLidarDiagInternalInfoResponse* response, void* client_data) {
  if (status != kLivoxLidarStatusSuccess) {
    printf("Query lidar internal info failed.\n");
    QueryLivoxLidarInternalInfo(handle, QueryInternalInfoCallback, nullptr);
    return;
  }

  if (response == nullptr) {
    return;
  }

  uint8_t host_point_ipaddr[4] {0};
  uint16_t host_point_port = 0;
  uint16_t lidar_point_port = 0;

  uint8_t host_imu_ipaddr[4] {0};
  uint16_t host_imu_data_port = 0;
  uint16_t lidar_imu_data_port = 0;

  uint16_t off = 0;
  for (uint8_t i = 0; i < response->param_num; ++i) {
    LivoxLidarKeyValueParam* kv = (LivoxLidarKeyValueParam*)&response->data[off];
    if (kv->key == kKeyLidarPointDataHostIpCfg) {
      memcpy(host_point_ipaddr, &(kv->value[0]), sizeof(uint8_t) * 4);
      memcpy(&(host_point_port), &(kv->value[4]), sizeof(uint16_t));
      memcpy(&(lidar_point_port), &(kv->value[6]), sizeof(uint16_t));
    } else if (kv->key == kKeyLidarImuHostIpCfg) {
      memcpy(host_imu_ipaddr, &(kv->value[0]), sizeof(uint8_t) * 4);
      memcpy(&(host_imu_data_port), &(kv->value[4]), sizeof(uint16_t));
      memcpy(&(lidar_imu_data_port), &(kv->value[6]), sizeof(uint16_t));
    }
    off += sizeof(uint16_t) * 2;
    off += kv->length;
  }

  printf("Host point cloud ip addr:%u.%u.%u.%u, host point cloud port:%u, lidar point cloud port:%u.\n",
      host_point_ipaddr[0], host_point_ipaddr[1], host_point_ipaddr[2], host_point_ipaddr[3], host_point_port, lidar_point_port);

  printf("Host imu ip addr:%u.%u.%u.%u, host imu port:%u, lidar imu port:%u.\n",
    host_imu_ipaddr[0], host_imu_ipaddr[1], host_imu_ipaddr[2], host_imu_ipaddr[3], host_imu_data_port, lidar_imu_data_port);

}

void LidarInfoChangeCallback(const uint32_t handle, const LivoxLidarInfo* info, void* client_data) {
  if (info == nullptr) {
    printf("lidar info change callback failed, the info is nullptr.\n");
    return;
  } 
  printf("LidarInfoChangeCallback Lidar handle: %u SN: %s lidar_ip: %s\n", handle, info->sn, info->lidar_ip);
  
  // set the work mode to kLivoxLidarNormal, namely start the lidar
  SetLivoxLidarWorkMode(handle, kLivoxLidarNormal, WorkModeCallback, nullptr);

  QueryLivoxLidarInternalInfo(handle, QueryInternalInfoCallback, nullptr);
  if(isSubstring(std::string(info->sn), global_sn)){
    std::cout << "开始修改ip" << std::endl;

    LivoxLidarIpInfo lidar_ip_info;
    strcpy(lidar_ip_info.ip_addr,  global_update_ip.c_str());
    strcpy(lidar_ip_info.net_mask, "255.255.255.0");
    strcpy(lidar_ip_info.gw_addr, "192.168.1.1");
    livox_status status = SetLivoxLidarIp(handle, &lidar_ip_info, SetIpInfoCallback, nullptr);
  }
}

int main(int argc, char *argv[])
{
  if (argc < 4)
  { // argc应该至少为2，因为argv[0]通常是程序的名称
    std::cerr << "请输入:[./update_ipv4 [config file] [SN] [LidarUpdataIp] " << std::endl;
    return 1; // 返回一个非零值表示出错
  }
  
  const std::string path = argv[1];
  global_sn = argv[2];
  global_update_ip = argv[3];
  
  LOG_INFO 
  << "即将设备SN: "   << global_sn << "\n"
  << "即将修改IP为: " << global_update_ip << "\n"
  << LOG_REND;

  // REQUIRED, to init Livox SDK2
  if (!LivoxLidarSdkInit(path.c_str())) {
    LOG_ERROR << "Livox Init Failed" << LOG_REND;
    LivoxLidarSdkUninit();
    return -1;
  }
  // REQUIRED, to get a handle to targeted lidar and set its work mode to NORMAL
  SetLivoxLidarInfoChangeCallback(LidarInfoChangeCallback, nullptr);

  while (true)
  {
    if(global_updata_state==1){
      LOG_INFO << "IP 修改完成：" <<  global_update_ip.c_str() << LOG_REND;
      break;
    }
    if(global_updata_state==2){
      LOG_INFO << "IP 修改失败：" <<  global_update_ip.c_str() << LOG_REND;
      break;
    }
    sleep(1);
  }

  LOG_INFO << "Livox Quick Start Demo End!" << LOG_REND;
  LivoxLidarSdkUninit();
  return 0;
}

/*
g++ update_ipv4.cpp -o update_ipv4.run -L./lib -llivox_lidar_sdk_shared -Wl,-rpath=./lib

./update_ipv4.run mid360_config.json 47MDL9J0020005 192.168.1.105 
*/