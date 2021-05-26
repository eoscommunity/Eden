import { useFetchedData, useMemberListByAccountNames } from "_app";
import * as InductionTable from "_app/ui/table";

import { getEndorsementsByInductionId } from "../../api";
import { getInductionRemainingTimeDays, getInductionStatus } from "../../utils";
import { Endorsement, Induction } from "../../interfaces";
import { InductionStatusButton } from "./induction-status-button";

interface Props {
    inductions: Induction[];
}

export const InviterInductions = ({ inductions }: Props) => (
    <InductionTable.Table
        columns={INVITER_INDUCTION_COLUMNS}
        data={getTableData(inductions)}
        tableHeader="People I'm inviting"
    />
);

const INVITER_INDUCTION_COLUMNS: InductionTable.Column[] = [
    {
        key: "invitee",
        label: "Invitee",
    },
    {
        key: "inviter_witnesses",
        label: "Inviter & Witnesses",
        className: "hidden md:flex",
    },
    {
        key: "time_remaining",
        label: "Time remaining",
        className: "hidden md:flex",
    },
    {
        key: "status",
        label: "Action/Status",
        type: InductionTable.DataTypeEnum.Action,
    },
];

const getTableData = (inductions: Induction[]): InductionTable.Row[] => {
    return inductions.map((induction) => {
        const [allEndorsements] = useFetchedData<Endorsement[]>(
            getEndorsementsByInductionId,
            induction.id
        );

        const endorsersAccounts =
            allEndorsements
                ?.map(
                    (endorsement: Endorsement): string => endorsement.endorser
                )
                .filter(
                    (endorsement: string) => endorsement !== induction.inviter
                ) || [];

        const endorsersMembers = useMemberListByAccountNames(endorsersAccounts);

        const endorsers =
            endorsersMembers
                .map(
                    (member, index) =>
                        member.data?.name || endorsersAccounts[index]
                )
                .join(", ") || "";

        const remainingTime = getInductionRemainingTimeDays(induction);

        const isEndorsed = allEndorsements?.find(
            (endorsement) => endorsement.inviter === induction.inviter
        )?.endorsed;

        return {
            key: induction.id,
            invitee: induction.new_member_profile.name || induction.invitee,
            inviter_witnesses: endorsers,
            time_remaining: remainingTime,
            status: (
                <InductionStatusButton
                    induction={induction}
                    status={getInductionStatus(induction, allEndorsements)}
                    canEndorse={!isEndorsed}
                />
            ),
        };
    });
};
