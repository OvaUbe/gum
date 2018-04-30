#include <gum/Visit.h>
#include <gum/concurrency/ConditionVariable.h>
#include <gum/concurrency/DummyCancellationHandle.h>
#include <gum/io/filesystem/FilesystemService.h>

#include <gtest/gtest.h>

#include <fstream>

using namespace gum;

namespace {

class Latch {
    Mutex mutex;
    ConditionVariable cv;
    bool done;

  public:
    Latch()
        : done() {}

    auto lock() const {
        return make_unique_ref<MutexLock>(mutex);
    }

    void wait(const Duration& duration) {
        cv.wait_for(mutex, duration, [this] { return done; }, *DummyCancellationHandle());
    }

    void signal() {
        MutexLock l(mutex);
        done = true;
        cv.broadcast();
    }
};

const String TestFilePath = "__FilesystemServiceTest.test";

void write_file(const String& path, const String& content) {
    std::ofstream os(path.c_str(), std::fstream::out);
    GUM_CHECK(os.is_open(), "Failed to open test file");

    std::copy(content.begin(), content.end(), std::ostream_iterator<char>(os, ""));
}

void write_test_file(const String& content) {
    write_file(TestFilePath, content);
}

template <typename Value_, typename Test_>
void for_each(std::initializer_list<Value_> list, const Test_& test) {
    for (const auto& value : list)
        test(value);
}
}

TEST(FilesystemServiceTest, ReadUntilEof) {
    for_each({1, 2, 5, 10, 20, 50}, [&](size_t concurrency) {
        for_each({1, 2, 10, 50, 128, 512, 999}, [&](size_t buffer_size) {
            for_each({"",
                      "abcdefgh",
                      "\n",
                      "a\0b"
                      "a\nb\nc\nd",
                      "a\nb\nc\nd\n"},
                     [&](const String& sample_content) {
                         write_test_file(sample_content);

                         FilesystemService service("fs_service", concurrency);
                         const IFileRef file = service.open_file(TestFilePath, FileMode::Read, buffer_size);
                         const asio::IAsyncByteStreamRef stream = file->get_stream();

                         String testee_content;
                         bool eof = false;

                         Latch latch;

                         const Token readConnection = stream->data_read().connect([&](const auto& result) {
                             visit(result,
                                   [&](ConstByteData data) { testee_content << String(data.begin(), data.end()); },
                                   [&](Eof) {
                                       eof = true;
                                       latch.signal();
                                   },
                                   [&](ExceptionRef ex) {
                                       ASSERT_TRUE(false) << "Read caught an error: " << to_string(ex).c_str();
                                       latch.signal();
                                   });
                         });

                         {
                             const auto l = latch.lock();

                             const Token readOpToken = stream->read();

                             latch.wait(Seconds(10));
                         }

                         ASSERT_TRUE(eof);
                         ASSERT_EQ(testee_content, sample_content);
                     });
        });
    });
}

TEST(FilesystemServiceTest, ReadCancelled) {
    for_each({1, 2, 5, 10, 20, 50}, [&](size_t concurrency) {
        const String sample_content = "abc";
        const String sample_part = "a";
        write_test_file(sample_content);

        FilesystemService service("fs_service", concurrency);
        const IFileRef file = service.open_file(TestFilePath, FileMode::Read, sample_part.size());
        const asio::IAsyncByteStreamRef stream = file->get_stream();

        String testee_content;
        bool eof = false;

        Latch latch;

        Token readOpToken;
        Mutex readOpTokenMutex;

        const Token readConnection = stream->data_read().connect([&](const auto& result) {
            visit(result,
                  [&](ConstByteData data) {
                      testee_content << String(data.begin(), data.end());
                      {
                          MutexLock l(readOpTokenMutex);
                          readOpToken.release();
                      }
                      latch.signal();
                  },
                  [&](Eof) {
                      eof = true;
                      latch.signal();
                  },
                  [&](ExceptionRef) { latch.signal(); });
        });

        {
            const auto l = latch.lock();
            {
                MutexLock l(readOpTokenMutex);
                readOpToken = stream->read();
            }
            latch.wait(Seconds(10));
        }

        ASSERT_FALSE(eof);
        ASSERT_EQ(testee_content, sample_part);
    });
}

TEST(FilesystemServiceTest, StreamOwnership) {
    for_each({1, 2, 5, 10, 20, 50}, [&](size_t concurrency) {
        const String sample_content = "abc";
        const String sample_part = "a";
        write_test_file(sample_content);

        FilesystemService service("fs_service", concurrency);
        IFilePtr file = service.open_file(TestFilePath, FileMode::Read, sample_part.size());
        asio::IAsyncByteStreamPtr stream = file->get_stream();

        String testee_content;
        bool eof = false;

        Latch latch;

        const Token readConnection = stream->data_read().connect([&](const auto& result) {
            visit(result,
                  [&](ConstByteData data) {
                      testee_content << String(data.begin(), data.end());

                      file.reset();
                      stream.reset();

                      latch.signal();
                  },
                  [&](Eof) {
                      eof = true;
                      latch.signal();
                  },
                  [&](ExceptionRef) { latch.signal(); });
        });

        {
            const auto l = latch.lock();

            const Token readOpToken = stream->read();

            latch.wait(Seconds(10));
        }

        ASSERT_FALSE(eof);
        ASSERT_EQ(testee_content, sample_part);
    });
}

TEST(FilesystemServiceTest, ReadSize) {
    for_each({1, 2, 5, 10, 50}, [&](size_t concurrency) {
        for_each({1, 2, 4, 50, 512, 999}, [&](size_t buffer_size) {
            for_each({std::make_pair("", "abc"),
                      std::make_pair("abcdefgh", "abcdefgh"),
                      std::make_pair("abcdefgh", "abc"),
                      std::make_pair("abcdefgh", "abcdefgh111")},
                     [&](const auto& sample) {
                         const String sample_content = sample.first;
                         const String sample_part = sample.second;
                         const String cropped_sample_part(sample_part.data(), sample_part.data() + std::min(sample_part.size(), sample_content.size()));

                         write_test_file(sample_content);

                         FilesystemService service("fs_service", concurrency);
                         const IFileRef file = service.open_file(TestFilePath, FileMode::Read, buffer_size);
                         const asio::IAsyncByteStreamRef stream = file->get_stream();

                         String testee_content;

                         Latch latch;

                         const Token readConnection = stream->data_read().connect([&](const auto& result) {
                             visit(result,
                                   [&](ConstByteData data) {
                                       testee_content << String(data.begin(), data.end());

                                       if (testee_content.size() == cropped_sample_part.size())
                                           latch.signal();
                                   },
                                   [&](Eof) { latch.signal(); },
                                   [&](ExceptionRef ex) {
                                       ASSERT_TRUE(false) << "Read caught an error: " << to_string(ex).c_str();
                                       latch.signal();
                                   });
                         });

                         {
                             const auto l = latch.lock();

                             const Token readOpToken = stream->read(sample_part.size());

                             latch.wait(Seconds(10));
                         }

                         ASSERT_EQ(testee_content, cropped_sample_part);
                     });
        });
    });
}
