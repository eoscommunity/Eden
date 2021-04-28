import dayjs from "dayjs";
import * as relativeTime from "dayjs/plugin/relativeTime";

import { getInductionStatus } from "inductions/utils";
import { getInduction } from "inductions/api";
import { useFetchedData } from "_app";
import * as InductionTable from "inductions/components/induction-lists/induction-table";
import { Endorsement, Induction, InductionStatus } from "../../interfaces";
import { InductionActionButton } from "./action-button";

dayjs.extend(relativeTime.default);

interface Props {
    endorsements: Endorsement[];
}

export const EndorserInductions = ({ endorsements }: Props) => (
    <InductionTable.Table
        columns={ENDORSER_INDUCTION_COLUMNS}
        data={getTableData(endorsements)}
        tableHeader="Invitations awaiting my endorsement"
    />
);

const ENDORSER_INDUCTION_COLUMNS: InductionTable.Column[] = [
    {
        key: "invitee",
        label: "Invitee",
    },
    {
        key: "inviter",
        label: "Inviter",
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

const getTableData = (endorsements: Endorsement[]): InductionTable.Row[] => {
    return endorsements.map((end) => {
        const [ind, isLoading] = useFetchedData<{
            induction: Induction;
            endorsements: Endorsement[];
        }>(getInduction, end.induction_id, false);
        const induction = ind?.induction;

        const remainingTime = induction
            ? dayjs().to(dayjs(induction.created_at).add(7, "day"), true)
            : "";

        return {
            key: `${end.induction_id}-${end.id}`,
            invitee: end.invitee,
            inviter: end.inviter,
            time_remaining: remainingTime,
            status: induction ? (
                <EndorserInductionStatus
                    induction={induction}
                    endorsement={end}
                />
            ) : (
                "Unknown"
            ),
        };
    });
};

interface EndorserInductionStatusProps {
    induction: Induction;
    endorsement: Endorsement;
}

const EndorserInductionStatus = ({
    induction,
    endorsement,
}: EndorserInductionStatusProps) => {
    const status = getInductionStatus(induction);
    switch (status) {
        case InductionStatus.waitingForProfile:
            return (
                <InductionActionButton
                    href={`/induction/${induction.id}`}
                    className="bg-gray-50"
                >
                    Waiting for profile
                </InductionActionButton>
            );
        case InductionStatus.waitingForVideo:
            return (
                <InductionActionButton
                    href={`/induction/${induction.id}`}
                    className="bg-blue-500 border-blue-500"
                    lightText
                >
                    Complete ceremony
                </InductionActionButton>
            );
        case InductionStatus.waitingForOtherEndorsement:
            if (endorsement.endorsed) {
                return (
                    <InductionActionButton
                        href={`/induction/${induction.id}`}
                        className="bg-gray-50"
                    >
                        Voting
                    </InductionActionButton>
                );
            }
            return (
                <InductionActionButton
                    href={`/induction/${induction.id}`}
                    className="bg-green-500"
                    lightText
                >
                    Endorse
                </InductionActionButton>
            );
        default:
            return <>Error</>;
    }
};
