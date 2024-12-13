#include "StandardIOStream.h"

StandardIOStream::StandardIOStream(StreamType type)
{
    switch (type)
    {
    case StreamType::Input:
        m_file = stdin;
        m_type = StreamType::Input;
        break;
    case StreamType::Output:
        m_file = stdout;
        m_type = StreamType::Output;
        break;
    case StreamType::Error:
        m_file = stderr;
        m_type = StreamType::Error;
        break;
    }
}

// Read implementation
ErrorOr<Bytes> StandardIOStream::read_some(Bytes buffer)
{
    size_t bytes_read = fread(buffer.data(), 1, buffer.size(), m_file);

    if (bytes_read == 0)
    {
        if (feof(m_file))
        {
            return Bytes(); // EOF
        }

        // Handle read error
        return Error::from_errno(errno);
    }

    return Bytes(buffer.data(), bytes_read);
}

// Write implementation
ErrorOr<size_t> StandardIOStream::write_some(ReadonlyBytes buffer)
{
    if (m_type == StreamType::Input)
    {
        return Error::from_errno(EBADF); // Cannot write to input stream
    }

    size_t bytes_written = fwrite(buffer.data(), 1, buffer.size(), m_file);

    if (bytes_written == 0)
    {
        return Error::from_errno(errno);
    }

    fflush(m_file);
    return bytes_written;
}

// Check if end of file
bool StandardIOStream::is_eof() const
{
    return feof(m_file);
}

// Check if stream is open
bool StandardIOStream::is_open() const
{
    return m_file != nullptr;
}

// Close stream (for standard streams, this is a no-op)
void StandardIOStream::close()
{
    // Do not actually close stdin/stdout/stderr
    // This is intentional to prevent disrupting standard IO
}

// Getter for stream type
StreamType StandardIOStream::get_type() const
{
    return m_type;
}