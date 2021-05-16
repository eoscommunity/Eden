import { useMemo } from "react";
import dayjs from "dayjs";

import { eosBlockTimestampISO, useUALAccount } from "_app";
import { MemberData } from "members";
import {
    Endorsement,
    Induction,
    InductionRole,
    InductionStatus,
} from "./interfaces";

const INDUCTION_EXPIRATION_DAYS = 7;

export const convertPendingProfileToMemberData = (
    induction: Induction
): MemberData => {
    return {
        templateId: 0,
        name: induction.new_member_profile.name,
        image: induction.new_member_profile.img,
        account: induction.invitee,
        bio: induction.new_member_profile.bio,
        socialHandles: JSON.parse(induction.new_member_profile.social || "{}"),
        inductionVideo: induction.video || "",
        attributions: induction.new_member_profile.attributions || "",
        createdAt: 0,
    };
};

export const getInductionStatus = (induction?: Induction) => {
    if (!induction) return InductionStatus.invalid;

    const isExpired = dayjs(eosBlockTimestampISO(induction.created_at))
        .add(INDUCTION_EXPIRATION_DAYS, "day")
        .isBefore(dayjs());

    return isExpired
        ? InductionStatus.expired
        : !induction.new_member_profile.name
        ? InductionStatus.waitingForProfile
        : !induction.video
        ? InductionStatus.waitingForVideo
        : InductionStatus.waitingForEndorsement;
};

export const getInductionRemainingTimeDays = (induction?: Induction) => {
    if (!induction) return "";

    const remainingTimeObj = dayjs(
        eosBlockTimestampISO(induction.created_at)
    ).add(INDUCTION_EXPIRATION_DAYS, "day");

    const isExpired = induction && remainingTimeObj.isBefore(dayjs());

    return isExpired ? "0 days" : dayjs().to(remainingTimeObj, true);
};

export const getInductionUserRole = (
    endorsements: Endorsement[],
    ualAccount?: any,
    induction?: Induction
): InductionRole => {
    if (!ualAccount) return InductionRole.Unauthenticated;
    if (!induction) return InductionRole.Unknown;
    const accountName = ualAccount.accountName;
    if (accountName === induction.invitee) return InductionRole.Invitee;
    if (accountName === induction.inviter) return InductionRole.Inviter;
    if (endorsements.find((e) => e.endorser === accountName)) {
        return InductionRole.Endorser;
    }
    return InductionRole.Unknown;
};

export const useInductionUserRole = (
    endorsements: Endorsement[],
    induction?: Induction
): InductionRole => {
    const [ualAccount] = useUALAccount();
    const userRole: InductionRole = useMemo(() => {
        if (!ualAccount) return InductionRole.Unauthenticated;
        if (!induction) return InductionRole.Unknown;
        const accountName = ualAccount.accountName;
        if (accountName === induction.invitee) return InductionRole.Invitee;
        if (accountName === induction.inviter) return InductionRole.Inviter;
        if (endorsements.find((e) => e.endorser === accountName)) {
            return InductionRole.Endorser;
        }
        return InductionRole.Unknown;
    }, [ualAccount, induction, endorsements]);
    return userRole;
};
