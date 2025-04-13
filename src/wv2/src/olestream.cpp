/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "olestream.h"
#include "wvlog.h"

#include <stdio.h> // FILE,...

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace wvWare;

OLEStream::OLEStream( OLEStorage* storage ) : m_storage( storage )
{
}

OLEStream::~OLEStream()
{
    if ( m_storage )
        m_storage->streamDestroyed( this );
}

void OLEStream::push()
{
    m_positions.push( tell() );
}

bool OLEStream::pop()
{
    if ( m_positions.empty() )
        return false;
    seek( m_positions.top(), G_SEEK_SET );
    m_positions.pop();
    return true;
}

OLEStreamReader::OLEStreamReader(OLEStorage* storage) : OLEStream(storage){}
OLEStreamReader::~OLEStreamReader(){}


OLEImageReader::OLEImageReader( OLEStreamReader& reader, unsigned int start, unsigned int limit ) :
    m_reader( reader ), m_start( start ), m_limit( limit ), m_position( start )
{
    if ( limit <= start )
        wvlog << "Error: The passed region is empty." << std::endl;
}

OLEImageReader::OLEImageReader( const OLEImageReader& rhs ) : m_reader( rhs.m_reader ), m_start( rhs.m_start ),
                                                              m_limit( rhs.m_limit ), m_position( rhs.m_position )
{
}

OLEImageReader::~OLEImageReader()
{
    // nothing to do
}

bool OLEImageReader::isValid() const
{
    return m_reader.isValid() && m_position >= m_start && m_position < m_limit;
}

bool OLEImageReader::seek( int offset, GSeekType whence )
{
    switch( whence ) {
        case G_SEEK_CUR:
            return updatePosition( m_position + offset );
        case G_SEEK_SET:
            return updatePosition( offset );
        case G_SEEK_END:
            return updatePosition( m_limit - 1 + offset );
        default:
            wvlog << "Error: Unknown GSeekType!" << std::endl;
            return false;
    }
}

int OLEImageReader::tell() const
{
    return static_cast<int>( m_position );
}

size_t OLEImageReader::size() const
{
    return m_limit - m_start;
}

size_t OLEImageReader::read( U8 *buffer, size_t length )
{
    m_reader.push();
    if ( !m_reader.seek( m_position, G_SEEK_SET ) ) {
        m_reader.pop();
        return 0;
    }

    size_t bytesRead = ( m_limit - m_position ) < length ? m_limit - m_position : length;
    if ( !m_reader.read( buffer, bytesRead ) ) {
        m_reader.pop();
        return 0;
    }
    //have to update our position in the stream
    unsigned int newpos = m_position + (unsigned int) bytesRead;
    wvlog << "new position is " << newpos << std::endl;
    if ( !updatePosition( newpos ) )
        wvlog << "error updating position in stream" << std::endl;
    m_reader.pop();
    return bytesRead;
}

bool OLEImageReader::updatePosition( unsigned int position )
{
    if ( m_start <= position && position < m_limit ) {
        m_position = position;
        return true;
    }
    return false;
}
