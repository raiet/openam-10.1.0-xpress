/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright (c) 2006 Sun Microsystems Inc. All Rights Reserved
 *
 * The contents of this file are subject to the terms
 * of the Common Development and Distribution License
 * (the License). You may not use this file except in
 * compliance with the License.
 *
 * You can obtain a copy of the License at
 * https://opensso.dev.java.net/public/CDDLv1.0.html or
 * opensso/legal/CDDLv1.0.txt
 * See the License for the specific language governing
 * permission and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL
 * Header Notice in each file and include the License file
 * at opensso/legal/CDDLv1.0.txt.
 * If applicable, add the following below the CDDL Header,
 * with the fields enclosed by brackets [] replaced by
 * your own identifying information:
 * "Portions Copyrighted [year] [name of copyright owner]"
 *
 * $Id: policy_entry.h,v 1.4 2008/06/25 08:14:34 qcheng Exp $
 *
 */ 

#ifndef __POLICY_ENTRY_H__
#define __POLICY_ENTRY_H__
#include <stdexcept>
#include <nspr.h>
#include <am_map.h>
#include <am_policy.h>
#include "mutex.h"
#include "internal_exception.h"
#include "tree.h"
#include "sso_token.h"
#include "xml_tree.h"
#include "xml_element.h"
#include "ref_cnt_ptr.h"
#include "mutex.h"
#include "scope_lock.h"
#include "naming_info.h"
#include "http.h"

BEGIN_PRIVATE_NAMESPACE
class ResourceName;
class PolicyNotificationHandler;
class PolicyEntry:public RefCntObj {
 private:
    am_resource_traits_t rsrcTraits;
    std::list<Tree *> forest;
    mutable Mutex lock;
    NamingInfo namingInfo;
    Http::CookieList cookies;
    bool dirty;
    KeyValueMap map;
    SSOToken ssoToken;

    Tree *getTree(const ResourceName &, bool usePatterns) const;

    bool create_policy_tree(XMLElement &, KVMRefCntPtr);

    bool append_policy_to_tree(Tree &, XMLElement &, KVMRefCntPtr);

    void addPolicyDecision(PolicyDecision *, bool);
    PolicyEntry(am_resource_traits_t rTraits): RefCntObj(),
				  rsrcTraits(rTraits), lock(),
				  cookies(), dirty(true), map() {
    }

 public:
    // Friends
    friend class Service;
    friend class PolicyNotificationHandler;

    virtual ~PolicyEntry();
    PolicyEntry(const SSOToken &, const KeyValueMap &,
	     const Properties &, am_resource_traits_t);
    inline bool isValid() {
	return !dirty;
    }

    inline const Http::CookieList getCookies() {
	return cookies;
    }

    const PDRefCntPtr getPolicyDecision(const std::string &resName) const;

    void getAllPolicyDecisions(const std::string &resName,
			       std::vector<PDRefCntPtr> &results) const;

    bool removePolicy(const ResourceName &);
    //
    // Mark this entry as dirty.
    // This function returns the old value of dirty flag.
    // The thread getting this value as false must clean the
    // entry from the tree.
    inline bool markDirty() {
	bool retVal = true;

	ScopeLock myLock(lock);
	if(dirty != true) {
	    dirty = true;
	    retVal = false;
	} else retVal = true;

	return retVal;
    }

    bool add_policy(XMLElement &, KVMRefCntPtr ptr);

    inline const SSOToken& getSSOToken() const {
	return ssoToken;
    }

    inline SSOToken& getSSOToken() {
	return ssoToken;
    }

    inline const NamingInfo &getNamingInfo() {
	return namingInfo;
    }
};

typedef RefCntPtr<PolicyEntry> PolicyEntryRefCntPtr;

END_PRIVATE_NAMESPACE
#endif
