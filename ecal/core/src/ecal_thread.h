/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2019 Continental Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ========================= eCAL LICENSE =================================
 */

/**
 * @brief  eCAL threading helper class
 **/

#pragma once

#include <ecal/ecal_eventhandle.h>

#include <atomic>
#include <thread>
#include <functional>

namespace eCAL {

/**
 * @brief eCAL 对线程的封装
 */
class CThread {
public:
    CThread() = default;
    virtual ~CThread();

    CThread(const CThread&)            = delete;
    CThread& operator=(const CThread&) = delete;
    CThread(CThread&& rhs)             = delete;
    CThread& operator=(CThread&& rhs)  = delete;

    int Start(int period, std::function<int()> ext_caller_);
    int Stop();
    int Fire() const;

    bool IsRunning() const { return (m_tdata.is_running); };

protected:
    struct ThreadData {

        std::thread          thread;              ///<<< 线程句柄
        int                  period{ 0 };         ///<<< 执行周期
        EventHandleT         event;               ///<<< 事件处理
        std::atomic<bool>    is_running{ false }; ///<<< 是否在运行中
        std::atomic<bool>    is_started{ false }; ///<<< 是否启动了
        std::atomic<bool>    do_stop{ false };    ///<<< 是否停止
        std::function<int()> ext_caller;          ///<<< 外部调用
    };

    // 线程数据
    struct ThreadData m_tdata;

    static void HelperThread(void* par_);
};
} // namespace eCAL
