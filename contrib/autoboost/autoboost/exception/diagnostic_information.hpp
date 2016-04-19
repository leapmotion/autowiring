//Copyright (c) 2006-2010 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_0552D49838DD11DD90146B8956D89593
#define UUID_0552D49838DD11DD90146B8956D89593
#if (__GNUC__*100+__GNUC_MINOR__>301) && !defined(AUTOBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && !defined(AUTOBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(push,1)
#endif

#include <autoboost/config.hpp>
#include <autoboost/exception/get_error_info.hpp>
#include <autoboost/exception/info.hpp>
#include <autoboost/utility/enable_if.hpp>
#ifndef AUTOBOOST_NO_RTTI
#include <autoboost/core/demangle.hpp>
#endif
#include <exception>
#include <sstream>
#include <string>

#ifndef AUTOBOOST_NO_EXCEPTIONS
#include <autoboost/exception/current_exception_cast.hpp>
namespace
autoboost
    {
    namespace
    exception_detail
        {
        std::string diagnostic_information_impl( autoboost::exception const *, std::exception const *, bool, bool );
        }

    inline
    std::string
    current_exception_diagnostic_information( bool verbose=true)
        {
        autoboost::exception const * be=current_exception_cast<autoboost::exception const>();
        std::exception const * se=current_exception_cast<std::exception const>();
        if( be || se )
            return exception_detail::diagnostic_information_impl(be,se,true,verbose);
        else
            return "No diagnostic information available.";
        }
    }
#endif

namespace
autoboost
    {
    namespace
    exception_detail
        {
        inline
        exception const *
        get_autoboost_exception( exception const * e )
            {
            return e;
            }

        inline
        exception const *
        get_autoboost_exception( ... )
            {
            return 0;
            }

        inline
        std::exception const *
        get_std_exception( std::exception const * e )
            {
            return e;
            }

        inline
        std::exception const *
        get_std_exception( ... )
            {
            return 0;
            }

        inline
        char const *
        get_diagnostic_information( exception const & x, char const * header )
            {
#ifndef AUTOBOOST_NO_EXCEPTIONS
            try
                {
#endif
                error_info_container * c=x.data_.get();
                if( !c )
                    x.data_.adopt(c=new exception_detail::error_info_container_impl);
                char const * di=c->diagnostic_information(header);
                AUTOBOOST_ASSERT(di!=0);
                return di;
#ifndef AUTOBOOST_NO_EXCEPTIONS
                }
            catch(...)
                {
                return 0;
                }
#endif
            }

        inline
        std::string
        diagnostic_information_impl( autoboost::exception const * be, std::exception const * se, bool with_what, bool verbose )
            {
            if( !be && !se )
                return "Unknown exception.";
#ifndef AUTOBOOST_NO_RTTI
            if( !be )
                be=dynamic_cast<autoboost::exception const *>(se);
            if( !se )
                se=dynamic_cast<std::exception const *>(be);
#endif
            char const * wh=0;
            if( with_what && se )
                {
                wh=se->what();
                if( be && exception_detail::get_diagnostic_information(*be,0)==wh )
                    return wh;
                }
            std::ostringstream tmp;
            if( be && verbose )
                {
                char const * const * f=get_error_info<throw_file>(*be);
                int const * l=get_error_info<throw_line>(*be);
                char const * const * fn=get_error_info<throw_function>(*be);
                if( !f && !l && !fn )
                    tmp << "Throw location unknown (consider using AUTOBOOST_THROW_EXCEPTION)\n";
                else
                    {
                    if( f )
                        {
                        tmp << *f;
                        if( int const * l=get_error_info<throw_line>(*be) )
                            tmp << '(' << *l << "): ";
                        }
                    tmp << "Throw in function ";
                    if( char const * const * fn=get_error_info<throw_function>(*be) )
                        tmp << *fn;
                    else
                        tmp << "(unknown)";
                    tmp << '\n';
                    }
                }
#ifndef AUTOBOOST_NO_RTTI
            if ( verbose )
                tmp << std::string("Dynamic exception type: ") <<
                    core::demangle((be?(AUTOBOOST_EXCEPTION_DYNAMIC_TYPEID(*be)):(AUTOBOOST_EXCEPTION_DYNAMIC_TYPEID(*se))).type_->name()) << '\n';
#endif
            if( with_what && se && verbose )
                tmp << "std::exception::what: " << wh << '\n';
            if( be )
                if( char const * s=exception_detail::get_diagnostic_information(*be,tmp.str().c_str()) )
                    if( *s )
                        return std::string(s);
            return tmp.str();
            }
        }

    template <class T>
    std::string
    diagnostic_information( T const & e, bool verbose=true )
        {
        return exception_detail::diagnostic_information_impl(exception_detail::get_autoboost_exception(&e),exception_detail::get_std_exception(&e),true,verbose);
        }

    inline
    char const *
    diagnostic_information_what( exception const & e, bool verbose=true ) throw()
        {
        char const * w=0;
#ifndef AUTOBOOST_NO_EXCEPTIONS
        try
            {
#endif
            (void) exception_detail::diagnostic_information_impl(&e,0,false,verbose);
            if( char const * di=exception_detail::get_diagnostic_information(e,0) )
                return di;
            else
                return "Failed to produce autoboost::diagnostic_information_what()";
#ifndef AUTOBOOST_NO_EXCEPTIONS
            }
        catch(
        ... )
            {
            }
#endif
        return w;
        }
    }

#if defined(_MSC_VER) && !defined(AUTOBOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(pop)
#endif
#endif
