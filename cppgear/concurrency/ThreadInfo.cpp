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

        class LightweightOwnerInfo {
            std::atomic<ThreadId> _owner_id;

        public:
            LightweightOwnerInfo()
                :   _owner_id(ThreadId())
            { }

            void acquire() {
                _owner_id.store(Thread::get_own_info()->get_id(), std::memory_order_release);
            }

            String to_string() const {
                return String() << "{ id: " << _owner_id.load(std::memory_order_acquire) << " }";
            }
        };


        class ExtendedOwnerInfo {
            ThreadInfoPtr       _owner_info;

            mutable std::mutex  _mutex;

        public:
            void acquire() {
                ThreadInfoRef const info = Thread::get_own_info();

                std::lock_guard<std::mutex> l(_mutex);
                _owner_info = info;
            }

            String to_string() const {
                ThreadInfoPtr info;
                {
                    std::lock_guard<std::mutex> l(_mutex);
                    info = _owner_info;
                }
                return info->to_string();
            }
        };

    }


    class OwnerInfo::Impl : public
#   ifdef CPPGEAR_CONCURRENCY_USES_LIGHTWEIGHT_OWNER_INFO
        LightweightOwnerInfo
#   else
        ExtendedOwnerInfo
#   endif
    { };


    OwnerInfo::OwnerInfo() { }


    OwnerInfo::~OwnerInfo() { }


    void OwnerInfo::acquire() {
        _impl->acquire();
    }


    String OwnerInfo::to_string() const {
        return _impl->to_string();
    }


    ThreadInfo::ThreadInfo(StringConstRef const& name)
        :   _id(std::this_thread::get_id()),
            _name(name)
    { }


    String ThreadInfo::to_string() const {
        return String() << "{ id: " << _id << ", name: " << _name << " }";
    }

}
