import {
    ActionButton,
    ActionButtonSize,
    ActionButtonType,
    useFetchedData,
    useMemberByAccountName,
} from "_app";
import * as InductionTable from "_app/ui/table";
import {
    getInduction,
    getInductionRemainingTimeDays,
    getInductionStatus,
} from "inductions";
import { Endorsement, Induction, InductionStatus } from "inductions/interfaces";

interface Props {
    endorsements: Endorsement[];
    isActiveCommunity?: boolean;
}

export const EndorserInductions = ({
    endorsements,
    isActiveCommunity,
}: Props) => (
    <InductionTable.Table
        columns={ENDORSER_INDUCTION_COLUMNS}
        data={getTableData(endorsements)}
        tableHeader={
            isActiveCommunity
                ? "Invitations awaiting my endorsement"
                : "Waiting on the following members"
        }
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
        const [induction] = useFetchedData<Induction>(
            getInduction,
            end.induction_id
        );

        const { data: inviter } = useMemberByAccountName(end.inviter);

        const remainingTime = getInductionRemainingTimeDays(induction);

        const invitee =
            induction && induction.new_member_profile.name
                ? induction.new_member_profile.name
                : end.invitee;

        return {
            key: `${end.induction_id}-${end.id}`,
            invitee,
            inviter: inviter ? inviter.name : end.inviter,
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
        case InductionStatus.Expired:
            return (
                <ActionButton
                    type={ActionButtonType.Disabled}
                    size={ActionButtonSize.S}
                    fullWidth
                    disabled
                >
                    Expired
                </ActionButton>
            );
        case InductionStatus.PendingProfile:
            return (
                <ActionButton
                    type={ActionButtonType.Neutral}
                    size={ActionButtonSize.S}
                    fullWidth
                    href={`/induction/${induction.id}`}
                >
                    Waiting for profile
                </ActionButton>
            );
        case InductionStatus.PendingCeremonyVideo:
            return (
                <ActionButton
                    type={ActionButtonType.InductionStatusCeremony}
                    size={ActionButtonSize.S}
                    fullWidth
                    href={`/induction/${induction.id}`}
                >
                    Complete ceremony
                </ActionButton>
            );
        case InductionStatus.PendingEndorsement:
            if (endorsement.endorsed) {
                return (
                    <ActionButton
                        type={ActionButtonType.Neutral}
                        size={ActionButtonSize.S}
                        fullWidth
                        href={`/induction/${induction.id}`}
                    >
                        Pending completion
                    </ActionButton>
                );
            }
            return (
                <ActionButton
                    type={ActionButtonType.InductionStatusAction}
                    size={ActionButtonSize.S}
                    fullWidth
                    href={`/induction/${induction.id}`}
                >
                    Review &amp; Endorse
                </ActionButton>
            );
        default:
            return <>Error</>;
    }
};
