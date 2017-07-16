/*
 * Copyright (c) Vladimir Golubev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cppgear/concurrency/ThreadInfo.h>
#include <cppgear/concurrency/Thread.h>

#include <atomic>
#include <mutex>

namespace cppgear {

    namespace {

        class LightweightThreadInfo {
            std::atomic<ThreadId> _owner_id;

        public:
            void acquire() {
                _owner_id.store(Thread::get_own_id(), std::memory_order_acquire);
            }

            String to_string() const {
                ThreadId const owner_id = _owner_id.load(std::memory_order_release);

                return String() << "{ id: " << owner_id << " }";
            }
        };


        class ExtendedThreadInfo {
            ThreadId            _owner_id;
            StringConstPtr      _owner_name;

            mutable std::mutex  _mutex;

        public:
            void acquire() {
                ThreadId const owner_id = Thread::get_own_id();
                StringConstRef const owner_name = Thread::get_own_name();

                std::lock_guard<std::mutex> l(_mutex);

                _owner_id = owner_id;
                _owner_name = owner_name;
            }

            String to_string() const {
                ThreadId owner_id;
                StringConstPtr owner_name;
                {
                    std::lock_guard<std::mutex> l(_mutex);

                    owner_id = _owner_id;
                    owner_name = _owner_name;
                }

                if (!owner_name)
                    return "<unavailable>";

                return String() << "{ id: " << owner_id << ", name: " << owner_name << " }";
            }
        };

    }


    class ThreadInfo::Impl {
#   ifdef CPPGEAR_CONCURRENCY_USES_LIGHTWEIGHT_THREAD_INFO
        using ImplImpl = LightweightThreadInfo;
#   else
        using ImplImpl = ExtendedThreadInfo;
#   endif

    private:
        ImplImpl _impl;

    public:
        void acquire() {
            _impl.acquire();
        }

        String to_string() const {
            return _impl.to_string();
        }
    };


    ThreadInfo::ThreadInfo() { }


    ThreadInfo::~ThreadInfo() { }


    void ThreadInfo::acquire() {
        _impl->acquire();
    }


    String ThreadInfo::to_string() const {
        return _impl->to_string();
    }

}
