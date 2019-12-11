//
// Created by xueyuegui on 19-8-25.
//

#include <boost/asio.hpp>

#include "SignalServer.h"
#include "MyThreadPool.h"

#include "Player/PlayerProxy.h"
#include "Common/MediaSource.h"
#include "rtsp/ffmpeg_src.h"

#include <thread>
#include "webrtctransport/Utils.hpp"

int main(){
    Utils::Crypto::ClassInit();
	std::thread th([]() {
		ffmepg_src();
		});
	th.detach();
    //std::string url = "rtsp://192.168.1.108:8554/live";
    //std::string url = "rtsp://49.234.178.152:8554/live";
    //std::string url = "rtsp://192.168.1.108:554/123";
	std::string url = "rtsp://127.0.0.1:8554/live";
    PlayerProxy::Ptr player(new PlayerProxy(DEFAULT_VHOST, "live", std::to_string(1).data()));
    //指定RTP over TCP(播放rtsp时有效)
    (*player)[kRtpType] = Rtsp::RTP_TCP;
    //开始播放，如果播放失败或者播放中止，将会自动重试若干次，重试次数在配置文件中配置，默认一直重试
    player->play(url);

    dtls::DtlsSocketContext::Init();
    MyThreadPool::GetInstance().Start(1,1);

    boost::asio::io_service ioService;
    SignalServer wserver(&ioService, 3000);
    wserver.Start();
    ioService.run();
}