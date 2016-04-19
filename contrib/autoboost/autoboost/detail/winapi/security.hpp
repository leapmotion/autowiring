//  security.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba
//  Copyright 2015 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_DETAIL_WINAPI_SECURITY_HPP
#define AUTOBOOST_DETAIL_WINAPI_SECURITY_HPP

#include <autoboost/detail/winapi/basic_types.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( AUTOBOOST_USE_WINDOWS_H )
extern "C" {
struct _ACL;
struct _SECURITY_DESCRIPTOR;
#if defined( AUTOBOOST_WINAPI_IS_MINGW )
typedef _SECURITY_DESCRIPTOR *PSECURITY_DESCRIPTOR;
#else
typedef autoboost::detail::winapi::PVOID_ PSECURITY_DESCRIPTOR;
#endif

AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
InitializeSecurityDescriptor(
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    autoboost::detail::winapi::DWORD_ dwRevision);
AUTOBOOST_SYMBOL_IMPORT autoboost::detail::winapi::BOOL_ WINAPI
SetSecurityDescriptorDacl(
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    autoboost::detail::winapi::BOOL_ bDaclPresent,
    ::_ACL* pDacl,
    autoboost::detail::winapi::BOOL_ bDaclDefaulted);
}
#endif

namespace autoboost {
namespace detail {
namespace winapi {

typedef PVOID_ PSID_;
typedef WORD_ SECURITY_DESCRIPTOR_CONTROL_, *PSECURITY_DESCRIPTOR_CONTROL_;

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _ACL {
    BYTE_ AclRevision;
    BYTE_ Sbz1;
    WORD_ AclSize;
    WORD_ AceCount;
    WORD_ Sbz2;
} ACL_, *PACL_;

typedef struct AUTOBOOST_DETAIL_WINAPI_MAY_ALIAS _SECURITY_DESCRIPTOR {
    BYTE_ Revision;
    BYTE_ Sbz1;
    SECURITY_DESCRIPTOR_CONTROL_ Control;
    PSID_ Owner;
    PSID_ Group;
    PACL_ Sacl;
    PACL_ Dacl;
} SECURITY_DESCRIPTOR_, *PISECURITY_DESCRIPTOR_;

typedef ::PSECURITY_DESCRIPTOR PSECURITY_DESCRIPTOR_;

using ::InitializeSecurityDescriptor;

AUTOBOOST_FORCEINLINE BOOL_ SetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR_ pSecurityDescriptor, BOOL_ bDaclPresent, PACL_ pDacl, BOOL_ bDaclDefaulted)
{
    return ::SetSecurityDescriptorDacl(pSecurityDescriptor, bDaclPresent, reinterpret_cast< ::_ACL* >(pDacl), bDaclDefaulted);
}

}
}
}

#endif // AUTOBOOST_DETAIL_WINAPI_SECURITY_HPP
