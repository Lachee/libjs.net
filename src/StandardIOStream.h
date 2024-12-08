#pragma once

#include <AK/Stream.h>
#include <cstdio>  // For FILE*, stdin, stdout, stderr
#include <cerrno>  // For errno

    /// Enumeration of standard stream types
    enum class StreamType {
        Input,   // Standard input stream
        Output,  // Standard output stream
        Error    // Standard error stream
    };

class StandardIOStream : public Stream {
private:
    FILE* m_file;        // Underlying file pointer
    StreamType m_type;   // Type of the stream

public:
    /// Construct a StandardIOStream with a specific stream type
    /// @param type The type of standard stream to wrap
    explicit StandardIOStream(StreamType type);

    /// Read some bytes from the stream
    /// @param buffer Bytes buffer to read into
    /// @return Number of bytes read or an error
    virtual ErrorOr<Bytes> read_some(Bytes buffer) override;

    /// Write some bytes to the stream
    /// @param buffer Bytes to write
    /// @return Number of bytes written or an error
    virtual ErrorOr<size_t> write_some(ReadonlyBytes buffer) override;

    /// Check if the stream has reached end of file
    /// @return True if EOF, false otherwise
    virtual bool is_eof() const override;

    /// Check if the stream is open
    /// @return True if the stream is open, false otherwise
    virtual bool is_open() const override;

    /// Close the stream (no-op for standard streams)
    virtual void close() override;

    /// Get the type of the stream
    /// @return The StreamType of this stream
    StreamType get_type() const;

    // Prevent copying
    StandardIOStream(const StandardIOStream&) = delete;
    StandardIOStream& operator=(const StandardIOStream&) = delete;

    // Allow move semantics
    StandardIOStream(StandardIOStream&& other) noexcept;
    StandardIOStream& operator=(StandardIOStream&& other) noexcept;

    /// Destructor
    ~StandardIOStream() = default;
};