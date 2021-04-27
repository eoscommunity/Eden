import { GetServerSideProps } from "next";

import { RawLayout } from "_app";
import { getMember, MemberCard, MemberCollections, MemberData } from "members";

interface Props {
    member?: MemberData;
}

export const MemberPage = ({ member }: Props) => {
    return member ? (
        <RawLayout title={`${member.name}'s Profile`}>
            <MemberCard member={member} />
            <MemberCollections
                edenAccount={member.edenAccount}
                templateId={member.templateId}
            />
        </RawLayout>
    ) : (
        <RawLayout title="Member not found">
            <div className="text-center max-w mt-4">
                This account is not an <strong>active eden member</strong>.
            </div>
        </RawLayout>
    );
};

export default MemberPage;

export const getServerSideProps: GetServerSideProps = async ({ params }) => {
    try {
        const edenAccount = params!.id as string;
        const member = await getMember(edenAccount);
        return { props: { member: member || null } };
    } catch (error) {
        console.error(">>> Fail to list eden members:" + error);
        return { props: { error: "Fail to list eden members" } };
    }
};
